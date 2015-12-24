//
// Copyright © 2015 D.E. Goodman-Wilson. All rights reserved.
//

#include <gtest/gtest.h>
#include <slack/slack.h>


class websocket :
        public slack::websocket
{

public:
    void receive_message(const std::string &message)
    {
        this->on_message(message);
    }


    virtual void start() override
    { }

    virtual void stop() override
    { }

    virtual void send_message(const std::string &message) override
    { }
};


TEST(rtm, hello)
{
    std::string event_str = R"(
    {
        "type": "hello"
    }
    )";

    auto socket = std::make_shared<websocket>();

    slack::real_time_client client{socket};
//    client.register_event_handler(slack::event::hello::type, [](std::shared_ptr<slack::base::event> event){
//       ASSERT_TRUE(static_cast<bool>(event));
//    });
    client.register_event_handler<slack::event::hello>([](std::shared_ptr<slack::event::hello> event) {
        ASSERT_TRUE(static_cast<bool>(event));
    });

    socket->receive_message(event_str);
}


TEST(rtm, user_typing)
{
    std::string event_str = R"(
    {
        "type": "user_typing",
        "channel": "C02ELGNBH",
        "user": "U024BE7LH"
    }
    )";

    auto socket = std::make_shared<websocket>();

    slack::real_time_client client{socket};
    client.register_event_handler<slack::event::user_typing>([](std::shared_ptr<slack::event::user_typing> event) {
        ASSERT_TRUE(static_cast<bool>(event));
        ASSERT_EQ("C02ELGNBH", *event->channel);
        ASSERT_EQ("U024BE7LH", *event->user);
    });
    socket->receive_message(event_str);
}

TEST(rtm, actually_connect_sync)
{
    auto resp = slack::rtm::start();
    auto socket = std::make_shared<slack::simple_websocket>(*resp.url);
    slack::real_time_client client{socket};
    bool done = false;

    //Set up a handler to handle that first hello. Notice that we, uh, really ought to time out in case we don't connect!
    client.register_event_handler<slack::event::hello>([&](std::shared_ptr<slack::event::hello> event) {
        ASSERT_TRUE(static_cast<bool>(event));
        done = true;
        client.stop();
    });

    client.start();

    ASSERT_TRUE(done);
}

TEST(rtm, actually_connect_async)
{
    auto resp = slack::rtm::start();
    auto socket = std::make_shared<slack::simple_websocket>(*resp.url);
    slack::real_time_client client{socket};
    bool done = false;

    //Set up a handler to handle that first hello. Notice that we, uh, really ought to time out in case we don't connect!
    client.register_event_handler<slack::event::hello>([&](std::shared_ptr<slack::event::hello> event) {
        ASSERT_TRUE(static_cast<bool>(event));
        done = true;
    });

    client.start_async();

    while (!done);
    client.stop();


    ASSERT_TRUE(done);
}

TEST(rtm, test_ping)
{
    auto resp = slack::rtm::start();
    auto socket = std::make_shared<slack::simple_websocket>(*resp.url);
    slack::real_time_client client{socket};
    bool done = false;

    //Set up a handler to handle that first hello. Notice that we, uh, really ought to time out in case we don't connect!
    client.register_event_handler<slack::event::pong>([&](std::shared_ptr<slack::event::pong> event) {
        ASSERT_TRUE(static_cast<bool>(event));
        done = true;
        client.stop();
    });

    client.start();

    ASSERT_TRUE(done);
}

//TODO tests for real time pinging!
