#pragma once

#include "stateful_widget.h"

template <typename T>
class StreamBuilder : public StatefulWidget
{
    class _State;

public:
    using Builder = Function<ref<Widget>(ref<BuildContext>, ref<AsyncSnapshot<T>>)>;
    StreamBuilder(ref<Stream<T>> stream, Builder builder, option<Key> key = nullptr)
        : StatefulWidget(key),
          stream(stream), builder(builder) {}

    finalref<Stream<T>> stream;
    finalref<Fn<ref<Widget>(ref<BuildContext>, ref<AsyncSnapshot<T>>)>> builder;
    ref<State<StatefulWidget>> createState() override;
};

template <typename T>
class StreamBuilder<T>::_State : public State<StreamBuilder<T>>
{
    using super = State<StreamBuilder<T>>;
    lateref<StreamSubscription<T>> _subscription;
    lateref<AsyncSnapshot<T>> _snapshot;

    Function<void(const T &)> _listener = [this](const T &value) { //
        _snapshot = AsyncSnapshot<T>::hasData(
            value,
            widget->stream->isClosed()
                ? AsyncSnapshot<>::ConnectionState::done
                : AsyncSnapshot<>::ConnectionState::active);
        this->setState([] {});

    };

    void _subscribe()
    {
        if (widget->stream->isClosed())
        {
            _snapshot = AsyncSnapshot<T>::noData(AsyncSnapshot<>::ConnectionState::none);
        }
        else
        {
            _snapshot = AsyncSnapshot<T>::noData(AsyncSnapshot<>::ConnectionState::active);
            auto stream = widget->stream;
            _subscription = stream->listen(_listener);
            stream->asFuture()->then([this, stream] { //
                if (this->mounted && this->_snapshot->state != AsyncSnapshot<>::ConnectionState::done)
                {
                    this->setState([this] { //
                        _snapshot = AsyncSnapshot<T>::stateWrapper(_snapshot, AsyncSnapshot<>::ConnectionState::done);
                    });
                }
            });
        }
    }

    void initState() override
    {
        super::initState();
        _subscribe();
    }

    void didWidgetUpdated(ref<StreamBuilder<T>> oldWidget) override
    {
        super::didWidgetUpdated(oldWidget);
        if (oldWidget->stream != widget->stream)
        {
            _subscription->cancel();
            _subscribe();
        }
    }

    void dispose() override
    {
        _subscription->cancel();
        super::dispose();
    }

    ref<Widget> build(ref<BuildContext> context) override
    {
        return widget->builder(context, this->_snapshot);
    }
};

template <typename T>
inline ref<State<StatefulWidget>> StreamBuilder<T>::createState()
{
    return Object::create<_State>();
}
