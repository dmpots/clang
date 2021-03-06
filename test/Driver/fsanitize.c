// RUN: %clang -target x86_64-linux-gnu -fcatch-undefined-behavior %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-UNDEFINED
// RUN: %clang -target x86_64-linux-gnu -fsanitize=undefined %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-UNDEFINED
// CHECK-UNDEFINED: "-fsanitize={{((signed-integer-overflow|integer-divide-by-zero|float-divide-by-zero|shift|unreachable|return|vla-bound|alignment|null|vptr|object-size|float-cast-overflow|bounds|enum|bool),?){15}"}}

// RUN: %clang -target x86_64-linux-gnu -fsanitize=integer %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-INTEGER
// CHECK-INTEGER: "-fsanitize={{((signed-integer-overflow|unsigned-integer-overflow|integer-divide-by-zero|shift),?){4}"}}

// RUN: %clang -target x86_64-linux-gnu -fsanitize=thread,undefined -fno-thread-sanitizer -fno-sanitize=float-cast-overflow,vptr,bool,enum %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-PARTIAL-UNDEFINED
// CHECK-PARTIAL-UNDEFINED: "-fsanitize={{((signed-integer-overflow|integer-divide-by-zero|float-divide-by-zero|shift|unreachable|return|vla-bound|alignment|null|object-size|bounds),?){11}"}}

// RUN: %clang -target x86_64-linux-gnu -fsanitize=address-full %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-ASAN-FULL
// CHECK-ASAN-FULL: "-fsanitize={{((address|init-order|use-after-return|use-after-scope),?){4}"}}

// RUN: %clang -target x86_64-linux-gnu -fsanitize=vptr -fno-rtti %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-VPTR-NO-RTTI
// RUN: %clang -target x86_64-linux-gnu -fsanitize=undefined -fno-rtti %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-VPTR-NO-RTTI
// CHECK-VPTR-NO-RTTI: '-fsanitize=vptr' not allowed with '-fno-rtti'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=address,thread -fno-rtti %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-SANA-SANT
// CHECK-SANA-SANT: '-fsanitize=address' not allowed with '-fsanitize=thread'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=address,memory -pie -fno-rtti %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-SANA-SANM
// CHECK-SANA-SANM: '-fsanitize=address' not allowed with '-fsanitize=memory'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=thread,memory -pie -fno-rtti %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-SANT-SANM
// CHECK-SANT-SANM: '-fsanitize=thread' not allowed with '-fsanitize=memory'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=memory,thread -pie -fno-rtti %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-SANM-SANT
// CHECK-SANM-SANT: '-fsanitize=thread' not allowed with '-fsanitize=memory'

// RUN: %clang -target x86_64-linux-gnu -faddress-sanitizer -fthread-sanitizer -fno-rtti %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-ASAN-TSAN
// CHECK-ASAN-TSAN: '-faddress-sanitizer' not allowed with '-fthread-sanitizer'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=init-order %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-ONLY-EXTRA-ASAN
// CHECK-ONLY-EXTRA-ASAN: argument '-fsanitize=init-order' only allowed with '-fsanitize=address'

// RUN: %clang -target x86_64-linux-gnu -fsanitize-memory-track-origins -pie %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-ONLY-TRACK-ORIGINS
// CHECK-ONLY-TRACK-ORIGINS: warning: argument unused during compilation: '-fsanitize-memory-track-origins'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=address %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-NO-EXTRA-TRACK-ORIGINS
// CHECK-NO-EXTRA-TRACK-ORIGINS-NOT: "-fsanitize-memory-track-origins"

// RUN: %clang -target x86_64-linux-gnu -fsanitize-address-zero-base-shadow -pie %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-ONLY-ASAN-ZERO-BASE-SHADOW
// CHECK-ONLY-ASAN-ZERO-BASE-SHADOW: warning: argument unused during compilation: '-fsanitize-address-zero-base-shadow'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=address -fsanitize=alignment -fsanitize=vptr -fno-sanitize=vptr %s -### 2>&1
// OK

// RUN: %clang -target x86_64-linux-gnu -fsanitize=memory -pie %s -### 2>&1
// OK

// RUN: %clang -target x86_64-linux-gnu -fsanitize=memory -fsanitize-memory-track-origins -pie %s -### 2>&1
// OK

// RUN: %clang -target x86_64-linux-gnu -fsanitize=vptr -fno-sanitize=vptr -fsanitize=undefined,address %s -### 2>&1
// OK

// RUN: %clang -target x86_64-linux-gnu -fsanitize=address -fsanitize-address-zero-base-shadow -pie %s -### 2>&1
// OK

// RUN: %clang -target x86_64-linux-gnu -fcatch-undefined-behavior -fthread-sanitizer -fno-thread-sanitizer -faddress-sanitizer -fno-address-sanitizer -fbounds-checking -### %s 2>&1 | FileCheck %s --check-prefix=CHECK-DEPRECATED
// CHECK-DEPRECATED: argument '-fcatch-undefined-behavior' is deprecated, use '-fsanitize=undefined' instead
// CHECK-DEPRECATED: argument '-fthread-sanitizer' is deprecated, use '-fsanitize=thread' instead
// CHECK-DEPRECATED: argument '-fno-thread-sanitizer' is deprecated, use '-fno-sanitize=thread' instead
// CHECK-DEPRECATED: argument '-faddress-sanitizer' is deprecated, use '-fsanitize=address' instead
// CHECK-DEPRECATED: argument '-fno-address-sanitizer' is deprecated, use '-fno-sanitize=address' instead
// CHECK-DEPRECATED: argument '-fbounds-checking' is deprecated, use '-fsanitize=bounds' instead

// RUN: %clang -target x86_64-linux-gnu -fsanitize=thread %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-TSAN-NO-PIE
// CHECK-TSAN-NO-PIE: invalid argument '-fsanitize=thread' only allowed with '-pie'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=memory %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-MSAN-NO-PIE
// CHECK-MSAN-NO-PIE: invalid argument '-fsanitize=memory' only allowed with '-pie'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=address -fsanitize-address-zero-base-shadow %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-ASAN-ZERO-BASE-SHADOW-NO-PIE
// CHECK-ASAN-ZERO-BASE-SHADOW-NO-PIE: invalid argument '-fsanitize-address-zero-base-shadow' only allowed with '-pie'

// RUN: %clang -target x86_64-linux-gnu -fsanitize=address -fsanitize-address-zero-base-shadow -fno-sanitize-address-zero-base-shadow %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-ASAN-ZERO-BASE-SHADOW-CANCEL
// CHECK-ASAN-ZERO-BASE-SHADOW-CANCEL-NOT: '-fsanitize-address-zero-base-shadow' only allowed with '-pie'

// RUN: %clang -target arm-linux-androideabi -fsanitize=address %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-ANDROID-ASAN-NO-PIE
// CHECK-ANDROID-ASAN-NO-PIE: AddressSanitizer on Android requires '-pie'

// RUN: %clang -target x86_64-linux-gnu %s -### 2>&1 | FileCheck %s --check-prefix=CHECK-RECOVER
// RUN: %clang -target x86_64-linux-gnu %s -fsanitize-recover -### 2>&1 | FileCheck %s --check-prefix=CHECK-RECOVER
// RUN: %clang -target x86_64-linux-gnu %s -fno-sanitize-recover -### 2>&1 | FileCheck %s --check-prefix=CHECK-NO-RECOVER
// RUN: %clang -target x86_64-linux-gnu %s -fno-sanitize-recover -fsanitize-recover -### 2>&1 | FileCheck %s --check-prefix=CHECK-RECOVER
// RUN: %clang -target x86_64-linux-gnu %s -fsanitize-recover -fno-sanitize-recover -### 2>&1 | FileCheck %s --check-prefix=CHECK-NO-RECOVER
// CHECK-RECOVER-NOT: sanitize-recover
// CHECK-NO-RECOVER: "-fno-sanitize-recover"
