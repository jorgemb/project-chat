#include <iostream>
#include <functional>
#include <chrono>
#include <future>
#include <asio.hpp>

class printer{
public:
    printer(asio::io_context& io) : 
        strand_(asio::make_strand(io)),
        time_(100),
        timer1_(io, time_),
        timer2_(io, time_),
        count_(0){

        // timer1_.async_wait(asio::bind_executor(strand_, 
        //     std::bind(&printer::print, this, std::ref(timer1_), 1)
        // ));

        // timer2_.async_wait(asio::bind_executor(strand_, 
        //     std::bind(&printer::print, this, std::ref(timer2_), 2)
        // ));

        timer1_.async_wait(std::bind(&printer::print, this, std::ref(timer1_), 1));
        timer2_.async_wait(std::bind(&printer::print, this, std::ref(timer2_), 2));

    }

    ~printer(){
        std::cout << "Final count is " << count_ << std::endl;
    }

    void print(asio::steady_timer& timer, int id){
        if(count_ < 10){
            {
                std::lock_guard lock(mutex_);
                std::cout << "Timer " << id << " in thread " << std::this_thread::get_id() << ": " << count_ << std::endl;
                ++count_;
            }

            timer.expires_at(timer.expiry() + time_);
            timer.async_wait(asio::bind_executor(strand_,
                std::bind(&printer::print, this, std::ref(timer), id)
            ));
        }
    }

private:
    std::chrono::milliseconds time_;
    asio::strand<asio::io_context::executor_type> strand_;
    asio::steady_timer timer1_, timer2_;
    int count_;

    static std::mutex mutex_;
};
std::mutex printer::mutex_;

int main() {
    using namespace std::placeholders;

    asio::io_context io;
    printer p(io);


    auto f1 = std::async(std::launch::async, [&io]() {io.run(); });
    auto f2 = std::async(std::launch::async, [&io]() {io.run(); });
    auto f3 = std::async(std::launch::async, [&io]() {io.run(); });
    io.run();

    std::cout << "End of execution" << std::endl;
    return 0;
}
