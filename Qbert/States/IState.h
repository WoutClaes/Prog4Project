#pragma once
#include <memory>

namespace qbert
{
    template<typename TOwner, typename TFrame>
    class IState
    {
    public:
        virtual ~IState() = default;
        virtual std::unique_ptr<IState<TOwner, TFrame>> Update(TOwner& owner, float deltaTime) = 0;
        virtual TFrame GetFrame() const = 0;
    };
}
