// RUN: %clang_cc1 %s -verify -fsyntax-only -std=gnu++98
typedef unsigned long long uint64_t;
typedef unsigned long long uint32_t;

uint64_t f0(uint64_t);
uint64_t f1(uint64_t, uint32_t);
uint64_t f2(uint64_t, ...);

static const uint64_t overflow = 1 * 4608 * 1024 * 1024; // expected-warning {{overflow in expression; result is 536870912 with type 'int'}}

uint64_t check_integer_overflows(int i) { //expected-note {{declared here}}
// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  uint64_t overflow = 4608 * 1024 * 1024,
// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
           overflow2 = (uint64_t)(4608 * 1024 * 1024),
// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
           overflow3 = (uint64_t)(4608 * 1024 * 1024 * i),
// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
           overflow4 =  (1ULL * ((4608) * ((1024) * (1024))) + 2ULL),
// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
           overflow5 = static_cast<uint64_t>(4608 * 1024 * 1024),
// expected-warning@+1 2{{overflow in expression; result is 536870912 with type 'int'}}
           multi_overflow = (uint64_t)((uint64_t)(4608 * 1024 * 1024) * (uint64_t)(4608 * 1024 * 1024));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  overflow += overflow2 = overflow3 = (uint64_t)(4608 * 1024 * 1024);
// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  overflow += overflow2 = overflow3 = 4608 * 1024 * 1024;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  overflow += overflow2 = overflow3 = static_cast<uint64_t>(4608 * 1024 * 1024);

  uint64_t not_overflow = 4608 * 1024 * 1024ULL;
  uint64_t not_overflow2 = (1ULL * ((uint64_t)(4608) * (1024 * 1024)) + 2ULL);

// expected-warning@+1 2{{overflow in expression; result is 536870912 with type 'int'}}
  overflow = 4608 * 1024 * 1024 ?  4608 * 1024 * 1024 : 0;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  overflow =  0 ? 0 : 4608 * 1024 * 1024;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  if (4608 * 1024 * 1024)
    return 0;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  if ((uint64_t)(4608 * 1024 * 1024))
    return 1;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  if (static_cast<uint64_t>(4608 * 1024 * 1024))
    return 1;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  if ((uint64_t)(4608 * 1024 * 1024))
    return 2;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  if ((uint64_t)(4608 * 1024 * 1024 * i))
    return 3;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  if ((1ULL * ((4608) * ((1024) * (1024))) + 2ULL))
    return 4;

// expected-warning@+1 2{{overflow in expression; result is 536870912 with type 'int'}}
  if ((uint64_t)((uint64_t)(4608 * 1024 * 1024) * (uint64_t)(4608 * 1024 * 1024)))
    return 5;

  switch (i) {
// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  case 4608 * 1024 * 1024:
    return 6;
// expected-warning@+1 {{overflow in expression; result is 537919488 with type 'int'}}
  case (uint64_t)(4609 * 1024 * 1024):
    return 7;
// expected-warning@+1 {{overflow in expression; result is 537919488 with type 'int'}}
  case 1 + static_cast<uint64_t>(4609 * 1024 * 1024):
    return 7;
// expected-error@+2 {{expression is not an integral constant expression}}
// expected-note@+1 {{read of non-const variable 'i' is not allowed in a constant expression}}
  case ((uint64_t)(4608 * 1024 * 1024 * i)):
    return 8;
// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  case ((1ULL * ((4608) * ((1024) * (1024))) + 2ULL)):
    return 9;
// expected-warning@+2 2{{overflow in expression; result is 536870912 with type 'int'}}
// expected-warning@+1 {{overflow converting case value to switch condition type (288230376151711744 to 0)}}
  case ((uint64_t)((uint64_t)(4608 * 1024 * 1024) * (uint64_t)(4608 * 1024 * 1024))):
    return 10;
  }

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  while (4608 * 1024 * 1024);

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  while ((uint64_t)(4608 * 1024 * 1024));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  while (static_cast<uint64_t>(4608 * 1024 * 1024));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  while ((uint64_t)(4608 * 1024 * 1024));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  while ((uint64_t)(4608 * 1024 * 1024 * i));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  while ((1ULL * ((4608) * ((1024) * (1024))) + 2ULL));

// expected-warning@+1 2{{overflow in expression; result is 536870912 with type 'int'}}
  while ((uint64_t)((uint64_t)(4608 * 1024 * 1024) * (uint64_t)(4608 * 1024 * 1024)));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  do { } while (4608 * 1024 * 1024);

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  do { } while ((uint64_t)(4608 * 1024 * 1024));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  do { } while (static_cast<uint64_t>(4608 * 1024 * 1024));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  do { } while ((uint64_t)(4608 * 1024 * 1024));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  do { } while ((uint64_t)(4608 * 1024 * 1024 * i));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  do { } while ((1ULL * ((4608) * ((1024) * (1024))) + 2ULL));

// expected-warning@+1 2{{overflow in expression; result is 536870912 with type 'int'}}
  do { } while ((uint64_t)((uint64_t)(4608 * 1024 * 1024) * (uint64_t)(4608 * 1024 * 1024)));

// expected-warning@+3 {{overflow in expression; result is 536870912 with type 'int'}}
// expected-warning@+3 {{overflow in expression; result is 536870912 with type 'int'}}
// expected-warning@+3 {{overflow in expression; result is 536870912 with type 'int'}}
  for (uint64_t i = 4608 * 1024 * 1024;
       (uint64_t)(4608 * 1024 * 1024);
       i += (uint64_t)(4608 * 1024 * 1024 * i));

// expected-warning@+3 {{overflow in expression; result is 536870912 with type 'int'}}
// expected-warning@+3 2{{overflow in expression; result is 536870912 with type 'int'}}
// expected-warning@+3 2{{overflow in expression; result is 536870912 with type 'int'}}
  for (uint64_t i = (1ULL * ((4608) * ((1024) * (1024))) + 2ULL);
       ((uint64_t)((uint64_t)(4608 * 1024 * 1024) * (uint64_t)(4608 * 1024 * 1024)));
       i = ((4608 * 1024 * 1024) + ((uint64_t)(4608 * 1024 * 1024))));

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  _Complex long long x = 4608 * 1024 * 1024;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  (__real__ x) = 4608 * 1024 * 1024;

// expected-warning@+1 {{overflow in expression; result is 536870912 with type 'int'}}
  (__imag__ x) = 4608 * 1024 * 1024;

// expected-warning@+4 {{overflow in expression; result is 536870912 with type 'int'}}
// expected-warning@+3 {{array index 536870912 is past the end of the array (which contains 10 elements)}}
// expected-note@+1 {{array 'a' declared here}}
  uint64_t a[10];
  a[4608 * 1024 * 1024] = 1i;

// expected-warning@+1 2{{overflow in expression; result is 536870912 with type 'int'}}
  return ((4608 * 1024 * 1024) + ((uint64_t)(4608 * 1024 * 1024)));
}
