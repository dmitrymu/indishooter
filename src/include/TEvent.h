// Public domain software (CC0).

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdexcept>

namespace Shooter
{
/** An event with some associated data.
*/
class SimpleEvent
{
  public:
    /// Callback to handle the data.
    using CallbackT = std::function<void()>;

    /// Create event in un-signalled state.
    SimpleEvent() : flag(false) {}

    /// Atomically execute callback and raise the event.
    void raise(const CallbackT &callback = []() {})
    {
        std::unique_lock l(mutex);
        if (flag) {
            throw std::runtime_error("SimpleEvent raised twice");
        }
        flag = true;
        callback();
        l.unlock();
        cv.notify_all();
    }

    /// Wait infinitely until the event is raised, then atomically 
    /// execute callback and clear the event state.
    void wait(const CallbackT &callback = []() {})
    {
        std::unique_lock l(mutex);
        cv.wait(l, [this] { return flag; });
        callback();
        flag = false;
        l.unlock();
        cv.notify_all();
    }

    /// Wait for specific time until the event is raised then atomically 
    /// execute callback and clear the event state.
    template <class Rep, class Period>
    bool wait_for(
        const std::chrono::duration<Rep, Period> &delay,
        const CallbackT &callback = []() {})
    {
        std::unique_lock l(mutex);
        const auto result = cv.wait_for(l, delay, [this] { return flag; });
        callback();
        flag = false;
        l.unlock();
        cv.notify_all();
        return result;
    }

  private:
    SimpleEvent(const SimpleEvent &) = delete;
    SimpleEvent(SimpleEvent &&) = delete;
    SimpleEvent &operator=(const SimpleEvent &) = delete;
    SimpleEvent &operator=(SimpleEvent &&) = delete;

    std::condition_variable cv;
    std::mutex mutex;
    bool flag;
};


/// Use SimpleEvent to associate data with the event.
template <typename TData>
class TEvent : public SimpleEvent
{
  public:
    TEvent() = default;

    void raise(const TData &src)
    {
        SimpleEvent::raise([this, src]() { data = src; });
    }

    TData wait()
    {
        TData result{};
        SimpleEvent::wait([this, &result]() { result = data; });
        return result;
    }

    template <class Rep, class Period>
    TData wait_for(const std::chrono::duration<Rep, Period> &delay)
    {
        TData result{};
        if (!SimpleEvent::wait_for(delay,
                                   [this, &result]() { result = data; })) {
            throw std::runtime_error("Wait for TEvent completion timed out");
        }
        return result;
    }

  private:
    TEvent(const TEvent &) = delete;
    TEvent(TEvent &&) = delete;
    TEvent &operator=(const TEvent &) = delete;
    TEvent &operator=(TEvent &&) = delete;

    TData data;
};

} // namespace Shooter