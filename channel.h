#ifndef CHANNEL_
#define CHANNEL_

#include <mutex>
#include <queue>
#include <thread>
#include <experimental/optional>

template <typename T>
class Channel {
 public:
  bool Send(T item) {
    std::lock_guard<std::mutex> l(mu_);

    // FIXME(an): Use maybe DCHECK from glog -> DCHECK(closed_)
    if (closed_) {
      return false;
    }

    list_.push(item);
    cv_.notify_all();
    return true;
  }

  std::experimental::optional<T> TryRecv() {
    std::unique_lock<std::mutex> l(mu_);

    cv_.wait(l, [&] { return list_.size() > 0 || closed_; });

    if (list_.empty()) {
      return {};
    }

    T entry = list_.front();
    list_.pop();

    return std::experimental::make_optional(entry);
  }

  void operator<<(T entry) { Send(entry); }

  void Close() {
    std::lock_guard<std::mutex> l(mu_);
    cv_.notify_all();
    closed_ = true;
  }

  bool Finished() {
    std::lock_guard<std::mutex> l(mu_);
    return closed_ && list_.empty();
  }

  class Iterator {
    public:
      explicit Iterator(Channel<T>* chan) : chan_(chan) {}

      std::experimental::optional<T> operator*() {
        return chan_->TryRecv();
      }

      void operator++() {
        // does nothing on purpose since this
        // iterator cannot advance.
      }

      bool operator!=(Iterator it) {
        return !chan_->Finished();
      }

    private:
      Channel<T>* chan_;
  };

  Iterator begin() {
    return Iterator(this);
  }

  Iterator end() {
    return Iterator(this);
  }

 private:
  std::queue<T> list_;
  std::mutex mu_;
  std::condition_variable cv_;
  bool closed_ = false;
};

#endif // CHANNEL_
