#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SocketIO
#include <boost/test/unit_test.hpp>

#include <masio.h>
#include <iostream>
#include <chrono>

//------------------------------------------------------------------------------
using namespace masio;
using namespace std;
namespace asio = boost::asio;
using namespace std::chrono;
using tcp = asio::ip::tcp;
using iterator = tcp::resolver::iterator;

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(test_accept_connect) {
  using masio::accept;

  asio::io_service ios;

  tcp::socket client(ios);
  tcp::socket server(ios);

  unsigned short port = 9090;

  auto p1 = accept(server, port);
  auto p2 = resolve(ios, "localhost", port)
         >= [&client](tcp::resolver::iterator i) {
              return connect(client, i);
            };

  auto p = all(p1, p2);

  bool executed = false;

  using Results = result<result<>, result<>>;

  p.execute([&executed](Results rs) {
     BOOST_REQUIRE(!rs.is_error());

     BOOST_REQUIRE(rs.value<0>().is_value());
     BOOST_REQUIRE(rs.value<1>().is_value());

     executed = true;
     });

  int poll_count = 0;

  while(ios.run_one()) {
    ++poll_count;
  }

  BOOST_REQUIRE(executed);
  BOOST_REQUIRE_EQUAL(poll_count, 3);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(test_connect_accept) {
  using masio::accept;
  using masio::wait;

  asio::io_service ios;

  tcp::socket client(ios);
  tcp::socket server(ios);

  unsigned short port = 9090;

  auto p1 = wait(ios, 100)
          > resolve(ios, "localhost", port)
         >= [&client](tcp::resolver::iterator i) {
              return connect(client, i);
            };
  auto p2 = accept(server, port);

  auto p = all(p1, p2);

  bool executed = false;

  using Results = result<result<>, result<>>;

  p.execute([&executed](Results rs) {
     BOOST_REQUIRE(!rs.is_error());

     BOOST_REQUIRE(rs.value<0>().is_value());
     BOOST_REQUIRE(rs.value<1>().is_value());

     executed = true;
     });

  int poll_count = 0;

  while(ios.run_one()) {
    ++poll_count;
  }

  BOOST_REQUIRE(executed);
  BOOST_REQUIRE_EQUAL(poll_count, 4);
}

//------------------------------------------------------------------------------
// TODO: Retry this test when not on boost 1.54 as it seems that socket.connect
//       always returns success in that version.
//BOOST_AUTO_TEST_CASE(test_cancel_connect_accept) {
//  using masio::accept;
//  using masio::wait;
//  using iterator = tcp::resolver::iterator;
//
//  asio::io_service ios;
//
//  tcp::socket client(ios);
//  tcp::socket server(ios);
//
//  unsigned short port = 9090;
//
//  action<none_t> p2ref;
//
//  auto p2 = all( accept(server, port)
//               , wait(ios, 100)
//                 >= [&p2ref](none_t) {
//                   p2ref.cancel();
//                   return success(none);
//                 })
//          > success(none);
//
//  p2ref = p2;
//
//  auto p = resolve(ios, "localhost", port)
//        >= [&client, p2](iterator i) {
//            return all(connect(client, i), p2);
//           };
//
//  bool executed = false;
//
//  using Results = result<result<none_t>, result<none_t>>;
//
//
//  p.execute([&executed](Results rs) {
//     BOOST_REQUIRE(!rs.is_error());
//
//     BOOST_REQUIRE(rs.value<0>().is_error());
//     BOOST_REQUIRE(rs.value<1>().is_value()); // TODO: Is this correct behaviour?
//
//     executed = true;
//     });
//
//  int poll_count = 0;
//
//  while(ios.run_one()) {
//    ++poll_count;
//  }
//
//  BOOST_REQUIRE(executed);
//  BOOST_REQUIRE_EQUAL(poll_count, 4);
//}
//
//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(test_send_receive) {
  using masio::accept;
  using masio::wait;
  using asio::buffer;

  asio::io_service ios;

  tcp::socket client(ios);
  tcp::socket server(ios);

  unsigned short port = 9090;

  std::string tx_buffer = "hello";
  std::string rx_buffer = "XXXXX";

  auto p1 = accept(server, port)
          > send(server, buffer(tx_buffer, tx_buffer.size()));

  auto p2 = (resolve(ios, "localhost", port)
         >= [&client](tcp::resolver::iterator i) {
              return connect(client, i);
            })
          > receive(client, buffer(&rx_buffer[0], rx_buffer.size()));

  auto p = all(p1, p2);

  bool executed = false;

  using Results = result<result<>, result<>>;

  p.execute([&executed, &rx_buffer, &tx_buffer](Results rs) {
     BOOST_REQUIRE(!rs.is_error());

     BOOST_REQUIRE(rs.value<0>().is_value());
     BOOST_REQUIRE(rs.value<1>().is_value());

     BOOST_REQUIRE_EQUAL(rx_buffer, tx_buffer);

     executed = true;
     });

  int poll_count = 0;

  while(ios.run_one()) {
    ++poll_count;
  }

  BOOST_REQUIRE(executed);
  BOOST_REQUIRE_EQUAL(poll_count, 5);
}

