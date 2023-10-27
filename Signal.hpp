#pragma once

#include <memory>

namespace impl
{
    template <typename T>
    class copy_on_write
    {
        std::shared_ptr<T> data;

    public:
        using data_type = T;
        template <typename... Args>
        copy_on_write(Args &&...args)
            : data(std::make_shared<T>(std::forward<Args>(args)...)) {}

        data_type &write()
        {
            if (data.use_count() > 1)
                data = std::make_shared<T>(*data);
            return *data;
        }

        data_type const &read()
        {
            return *data;
        }

        copy_on_write copy() const
        {
            return copy_on_write(*this);
        }
    };

    struct SlotBase
    {
        virtual void disconnect() = 0;
        virtual ~SlotBase() = default;
    };

} // namespace impl

class Connection
{
    std::weak_ptr<impl::SlotBase> slot_ptr;
public:
    explicit Connection(std::weak_ptr<impl::SlotBase> slot_ptr)
        : slot_ptr(slot_ptr) {}
    Connection(Connection const &) = delete;
    Connection(Connection &&) = default;
    Connection &operator=(Connection const &) = delete;
    Connection &operator=(Connection &&other)
    {
        disconnect();
        slot_ptr = std::move(other.slot_ptr);
        return *this;
    }
    ~Connection() { disconnect(); }
    void disconnect()
    {
        auto locked_ptr = slot_ptr.lock();
        if (locked_ptr)
            locked_ptr->disconnect();
    }
};

template<class R, class... Args>
class Signal {
    struct Slot: impl::SlotBase {
        Signal &signal;
        std::function<R(Args...)> fn;
        Slot(Signal &signal, std::function<R(Args...)> &&fn)
            : signal(signal), fn(std::move(fn)) {}
        void disconnect() override { signal.disconnect(this); }
    };
    friend class Slot;
    void disconnect(impl::SlotBase *slot_ptr)
    {
        auto pos = std::find_if(connection_list.read().begin(),
                                connection_list.read().end(),
                                [slot_ptr](auto const &x)
                                { return x.get() == slot_ptr; });
        if (pos != connection_list.read().end())
        {
            auto index = pos - connection_list.read().begin();
            auto &vec = connection_list.write();
            vec.erase(vec.begin() + index);
        }
    }

public:
    Connection connect(std::function<R(Args...)> &&fn)
    {
        connection_list.write().emplace_back(std::make_shared<Slot>(*this, std::move(fn)));
        return Connection(connection_list.read().back());
    }
    void operator()(Args... args) const
    {
        auto copy = connection_list.copy();
        for (auto const &slot : copy.read())
            slot->fn(args...);
    }

private:
    impl::copy_on_write<std::vector<std::shared_ptr<Slot>>> connection_list;
};
