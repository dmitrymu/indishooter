#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdexcept>

namespace Shooter
{
class SimpleEvent
{
  public:
    using CallbackT = std::function<void()>;

    SimpleEvent() : flag(false) {}

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

    void wait(const CallbackT &callback = []() {})
    {
        std::unique_lock l(mutex);
        cv.wait(l, [this] { return flag; });
        callback();
        flag = false;
        l.unlock();
        cv.notify_all();
    }

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