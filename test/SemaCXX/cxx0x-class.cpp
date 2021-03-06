// RUN: %clang_cc1 -fsyntax-only -verify -std=c++11 -Wno-error=static-float-init %s 

int vs = 0;

class C {
public:
  struct NestedC {
    NestedC(int);
  };

  int i = 0;
  static int si = 0; // expected-error {{non-const static data member must be initialized out of line}}
  static const NestedC ci = 0; // expected-error {{static data member of type 'const C::NestedC' must be initialized out of line}}
  static const int nci = vs; // expected-error {{in-class initializer for static data member is not a constant expression}}
  static const int vi = 0;
  static const volatile int cvi = 0; // expected-error {{static const volatile data member must be initialized out of line}}
};

namespace rdar8367341 {
  float foo(); // expected-note 2{{here}}

  struct A {
    static const float x = 5.0f; // expected-warning {{requires 'constexpr'}}
    static const float y = foo(); // expected-warning {{requires 'constexpr'}} expected-error {{constexpr variable 'y' must be initialized by a constant expression}} expected-note {{non-constexpr function 'foo'}}
    static constexpr float x2 = 5.0f;
    static constexpr float y2 = foo(); // expected-error {{must be initialized by a constant expression}} expected-note {{non-constexpr function 'foo'}}
  };
}


namespace Foo {
  // Regression test -- forward declaration of Foo should not cause error about
  // nonstatic data member.
  class Foo;
  class Foo {
    int x;
    int y = x;
  };
}
