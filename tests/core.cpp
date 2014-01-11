#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>

#include <boost/asio.hpp>
#include <masio.h>
#include <iostream>

using namespace masio;
using namespace std;
namespace asio = boost::asio;
typedef shared_ptr<Canceler> CancelerPtr;

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(zero_binds) {
  CancelerPtr canceler = make_shared<Canceler>();

  auto p = success<int>(10);

  bool executed = false;

  p.run(canceler, [&executed](Error<int> i) {
     BOOST_REQUIRE(!i.is_error());
     BOOST_REQUIRE_EQUAL(*i, 10);
     executed = true;
     });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(one_bind) {
  CancelerPtr canceler = make_shared<Canceler>();

  auto p1 = success<int>(10);
  auto p2 = p1 >= [](int i) { return success(i+1); };

  bool executed = false;

  p2.run(canceler, [&executed](Error<int> i) {
      BOOST_REQUIRE(!i.is_error());
      BOOST_REQUIRE_EQUAL(*i, 11);
      executed = true;
      });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(one_bind_one_var) {
  CancelerPtr canceler = make_shared<Canceler>();

  auto p = success(10) >= [](int i) { return success(i+1); };

  bool executed = false;

  p.run(canceler, [&executed](Error<int> i) {
     BOOST_REQUIRE(!i.is_error());
     BOOST_REQUIRE_EQUAL(*i, 11);
     executed = true;
     });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(two_binds) {
  CancelerPtr canceler = make_shared<Canceler>();

  auto p1 = success<int>(10);
  auto p2 = p1 >= [](int i) { return success(i+1); };
  auto p3 = p2 >= [](int i) { return success(i+1); };

  bool executed = false;

  p3.run(canceler, [&executed](Error<int> i) {
      BOOST_REQUIRE(!i.is_error());
      BOOST_REQUIRE_EQUAL(*i, 12);
      executed = true;
      });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(two_binds_one_var) {
  CancelerPtr canceler = make_shared<Canceler>();

  auto p = success<int>(10) >= [](int i)
           { return success(i+1); } >= [](int i)
           { return success(i+1); };

  bool executed = false;

  p.run(canceler, [&executed](Error<int> i) {
     BOOST_REQUIRE(!i.is_error());
     BOOST_REQUIRE_EQUAL(*i, 12);
     executed = true;
     });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
// Fail right at the beginning.
BOOST_AUTO_TEST_CASE(fail0) {
  CancelerPtr canceler = make_shared<Canceler>();

  auto p = fail<int>(asio::error::operation_aborted)
        >= [](int a) { return success<int>(a); };

  bool executed = false;

  p.run(canceler, [&executed](Error<int> i) {
     BOOST_REQUIRE(i.is_error());
     BOOST_REQUIRE_EQUAL(i.error(), asio::error::operation_aborted);
     executed = true;
     });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(store_in_task_zero_binds) {
  CancelerPtr canceler = make_shared<Canceler>();

  task<int> p = success<int>(10);

  bool executed = false;

  p.run(canceler, [&executed](Error<int> i) {
     BOOST_REQUIRE(!i.is_error());
     BOOST_REQUIRE_EQUAL(*i, 10);
     executed = true;
     });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(store_in_task_one_bind) {
  CancelerPtr canceler = make_shared<Canceler>();

  task<int> p1 = success<int>(10);
  task<int> p2 = p1 >= [](int i) { return success(i+1); };

  bool executed = false;

  p2.run(canceler, [&executed](Error<int> i) {
      BOOST_REQUIRE(!i.is_error());
      BOOST_REQUIRE_EQUAL(*i, 11);
      executed = true;
      });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(store_in_task_one_bind_one_var) {
  CancelerPtr canceler = make_shared<Canceler>();

  task<int> p = success(10) >= [](int i) { return success(i+1); };

  bool executed = false;

  p.run(canceler, [&executed](Error<int> i) {
     BOOST_REQUIRE(!i.is_error());
     BOOST_REQUIRE_EQUAL(*i, 11);
     executed = true;
     });

  BOOST_CHECK(executed);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(store_in_task_two_binds) {
  CancelerPtr canceler = make_shared<Canceler>();

  task<int> p1 = success<int>(10);
  task<int> p2 = p1 >= [](int i) { return success(i+1); };
  task<int> p3 = p2 >= [](int i) { return success(i+1); };

  bool executed = false;

  p3.run(canceler, [&executed](Error<int> i) {
      BOOST_REQUIRE(!i.is_error());
      BOOST_REQUIRE_EQUAL(*i, 12);
      executed = true;
      });

  BOOST_CHECK(executed);
}

