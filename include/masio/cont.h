#ifndef __MASIO_CONT_H__
#define __MASIO_CONT_H__

#include "canceler.h"

namespace masio {

template<class> struct Lambda;

template<class A> struct Cont : public std::enable_shared_from_this<Cont<A>> {
  typedef std::shared_ptr<Canceler>                     CancelerPtr;
  typedef std::function<void(Error<A>)>                 Rest;
  typedef std::function<void(const CancelerPtr&, Rest)> Run;
  typedef std::shared_ptr<Cont<A>>                      Ptr;

  // [s -> (Ea -> r) -> r]
  virtual void run(const CancelerPtr& s, const Rest& rest) const = 0;

  // [s -> (Ea -> r) -> r] ->
  // a -> [s -> (Eb -> r) -> r] ->
  // [s -> (Eb -> r) -> r]
  template<class B>
  typename Cont<B>::Ptr bind(std::function<typename Cont<B>::Ptr(const A&)> f) {
    using namespace std;
    typedef typename Cont<B>::Rest BRest;

    auto self = this->shared_from_this();

    // [s -> (Eb -> r) -> r]
    return make_shared<Lambda<B>>(([self, f](const CancelerPtr& s, BRest brest) {
        using namespace boost::asio;

        self->run(s, [s, brest,f, self](const Error<A> ea) {
            if (s->canceled()) {
              brest(typename Error<B>::Fail{error::operation_aborted});
            }
            else if (ea.is_error()) {
              brest(typename Error<B>::Fail{ea.error()});
            }
            else {
              f(ea.value())->run(s, brest);
            }
          });
        }));
  }
};

} // masio namespace

#endif // ifndef __MASIO_CONT_H__
