#include "SDLSoundSystem.h"

#include <SDL3/SDL.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

namespace dae
{
    struct SoundRequest
    {
        SoundId id;
        float   volume;
    };

    struct LoadedSound
    {
        std::vector<Uint8> buffer;
        SDL_AudioSpec      spec{};
    };

    class SDLSoundSystem::Impl
    {
    public:
        Impl()
        {
            SDL_AudioSpec desired{};
            desired.format   = SDL_AUDIO_S16;
            desired.channels = 2;
            desired.freq     = 44100;

            m_DeviceId = SDL_OpenAudioDevice(
                SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);

            if (m_DeviceId == 0)
            {
                std::cerr << "SDLSoundSystem: SDL_OpenAudioDevice failed: "
                          << SDL_GetError() << "\n";
                return;
            }
            SDL_Log("SDLSoundSystem: audio device opened, id=%u", m_DeviceId);
            m_Initialized = true;

            UpdateDeviceVolume();

            m_Worker = std::thread(&Impl::WorkerLoop, this);
        }

        ~Impl()
        {
            {
                std::lock_guard lock(m_Mutex);
                m_Quit = true;
            }
            m_CV.notify_one();

            if (m_Worker.joinable())
                m_Worker.join();

            if (m_DeviceId)
                SDL_CloseAudioDevice(m_DeviceId);
        }

        SoundId RegisterSound(const std::string& filename)
        {
            auto it = m_FileToId.find(filename);
            if (it != m_FileToId.end())
                return it->second;

            const SoundId newId = static_cast<SoundId>(m_NextId++);
            m_FileToId[filename] = newId;
            m_IdToFile[newId]    = filename;
            return newId;
        }

        void Play(SoundId id, float volume)
        {
            if (!m_Initialized || m_Muted) return;
            {
                std::lock_guard lock(m_Mutex);
                m_Queue.push({ id, volume });
            }
            m_CV.notify_one();
        }

        void SetMuted(bool muted)
        {
            std::lock_guard lock(m_Mutex);
            m_Muted = muted;
            UpdateDeviceVolume();
        }

        bool IsMuted() const 
        {
            std::lock_guard lock(m_Mutex);
            return m_Muted;
        }

        void SetVolume(float volume)
        {
            std::lock_guard lock(m_Mutex);
            m_Volume = volume;
            UpdateDeviceVolume();
        }

        float GetVolume() const
        {
            std::lock_guard lock(m_Mutex);
            return m_Volume;
        }

    private:
        struct ActiveStream
        {
            SDL_AudioStream* stream;
            std::chrono::steady_clock::time_point endTime;
        };

        void UpdateDeviceVolume()
        {
            if (m_Initialized && m_DeviceId != 0)
            {
                float actualGain = m_Muted ? 0.0f : m_Volume;
                SDL_SetAudioDeviceGain(m_DeviceId, actualGain);
            }
        }

        void WorkerLoop()
        {
            while (true)
            {
                SoundRequest req{};
                bool hasRequest = false;
                {
                    std::unique_lock lock(m_Mutex);
                    m_CV.wait_for(lock, std::chrono::milliseconds(10), [this]
                        {
                            return m_Quit || !m_Queue.empty();
                        });

                    if (m_Quit && m_Queue.empty()) break;

                    if (!m_Queue.empty())
                    {
                        req = m_Queue.front();
                        m_Queue.pop();
                        hasRequest = true;
                    }
                }

                if (hasRequest)
                {
                    ProcessRequest(req);
                }

                CleanUpStreams();
            }

            for (auto& active : m_ActiveStreams)
            {
                SDL_UnbindAudioStream(active.stream);
                SDL_DestroyAudioStream(active.stream);
            }
            m_ActiveStreams.clear();
        }

        void CleanUpStreams()
        {
            auto now = std::chrono::steady_clock::now();
            for (auto it = m_ActiveStreams.begin(); it != m_ActiveStreams.end(); )
            {
                if (now >= it->endTime)
                {
                    SDL_UnbindAudioStream(it->stream);
                    SDL_DestroyAudioStream(it->stream);
                    it = m_ActiveStreams.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        void ProcessRequest(const SoundRequest& req)
        {
            SDL_Log("ProcessRequest: id=%u vol=%.2f", req.id, req.volume);

            if (m_Sounds.find(req.id) == m_Sounds.end())
            {
                auto fileIt = m_IdToFile.find(req.id);
                if (fileIt == m_IdToFile.end()) return;

                SDL_AudioSpec spec{};
                Uint8* rawBuf = nullptr;
                Uint32 rawLen = 0;

                if (!SDL_LoadWAV(fileIt->second.c_str(), &spec, &rawBuf, &rawLen))
                {
                    std::cerr << "SDLSoundSystem: SDL_LoadWAV failed for '"
                              << fileIt->second << "': " << SDL_GetError() << "\n";
                    return;
                }

                SDL_Log("Loaded WAV: %s bufsize=%u", fileIt->second.c_str(), rawLen);

                LoadedSound sound;
                sound.spec = spec;
                sound.buffer.assign(rawBuf, rawBuf + rawLen);
                SDL_free(rawBuf);
                m_Sounds[req.id] = std::move(sound);
            }

            const LoadedSound& sound = m_Sounds[req.id];

            SDL_AudioStream* stream = SDL_CreateAudioStream(&sound.spec, nullptr);
            SDL_Log("Stream created: %s", stream ? "ok" : "FAILED");
            if (!stream)
            {
                std::cerr << "SDLSoundSystem: SDL_CreateAudioStream failed: "
                          << SDL_GetError() << "\n";
                return;
            }

            SDL_SetAudioStreamGain(stream, req.volume);
            SDL_BindAudioStream(m_DeviceId, stream);
            SDL_PutAudioStreamData(stream,
                sound.buffer.data(),
                static_cast<int>(sound.buffer.size()));
            SDL_FlushAudioStream(stream);

            const int bytesPerSample = SDL_AUDIO_BYTESIZE(sound.spec.format)
                * sound.spec.channels;
            const float durationSec = static_cast<float>(sound.buffer.size())
                / static_cast<float>(sound.spec.freq * bytesPerSample);
            const int sleepMs = static_cast<int>(durationSec * 1000.f) + 50;

            auto endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(sleepMs);
            m_ActiveStreams.push_back({ stream, endTime });
        }

        bool m_Initialized{ false };
        bool m_Muted{ false };
        float m_Volume{ 1.0f };
        SDL_AudioDeviceID m_DeviceId{ 0 };

        unsigned int m_NextId{ 1 };
        std::unordered_map<std::string, SoundId> m_FileToId{};
        std::unordered_map<SoundId, std::string> m_IdToFile{};
        std::unordered_map<SoundId, LoadedSound> m_Sounds{};

        std::queue<SoundRequest> m_Queue{};
        std::vector<ActiveStream> m_ActiveStreams{};
        mutable std::mutex m_Mutex{};
        std::condition_variable m_CV{};
        std::thread m_Worker{};
        bool m_Quit{ false };
    };

    SDLSoundSystem::SDLSoundSystem()
        : m_pImpl(std::make_unique<Impl>()) {}

    SDLSoundSystem::~SDLSoundSystem() = default;

    SoundId SDLSoundSystem::RegisterSound(const std::string& filename)
    { 
        return m_pImpl->RegisterSound(filename); 
    }

    void SDLSoundSystem::Play(SoundId id, float volume)
    { 
        m_pImpl->Play(id, volume); 
    }

    void SDLSoundSystem::SetMuted(bool muted)
    { 
        m_pImpl->SetMuted(muted); 
    }

    bool SDLSoundSystem::IsMuted() const
    { 
        return m_pImpl->IsMuted(); 
    }

    void SDLSoundSystem::SetVolume(float volume)
    {
        m_pImpl->SetVolume(volume);
    }

    float SDLSoundSystem::GetVolume() const
    {
        return m_pImpl->GetVolume();
    }
}
