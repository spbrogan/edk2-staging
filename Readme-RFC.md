# Testing

Project Mu supports a few types of testing and this page will help provide some high level info and links for more information.

## Testing Infrastructure - Definitions and Descriptions

### Host-Based Unit Tests (cmocka)

Host-based unit tests let you compile your unit tests to run as an application in the host.  This method can also leverage "mocking" objects/functions so that unit tests can validate functionality in isolation and force unexpected edge cases.  These unit tests call C functions directly with known parameter sets to force good and bad conditions.  They are linked directly against C code in either a library instance or module. These tests leverage a UnitTest Library.  For Project Mu we have chosen cmocka.

There are a few different types of these tests...

#### Library/Protocol Interface

An interface unit test should be written such that it would be valid against any implementation of the library or protocol. For example, an interface test of DxeImageVerification lib should produce valid results for both an OpenSSL implementation and an implementation using another crypto provider. This is not going to always be true for proprietary implementations of certain libraries, such as libraries that intentionally remove some defined functionality, but it should be the ultimate goal of all interface tests.

*Note:* This assertion may not hold true for Null library implementations, which will usually return fixed values.

#### Library/Protocol Implementation

These tests expect a particular implementation of a library or protocol interface and may have far more detailed test cases. For example, an implementation test could be written for both a Null instance and/or a proprietary implementation of a given library.

#### Functionality Tests

These tests are written against larger chunks of business logic and may not have obvious divisions along a single library or protocol interface. Most of these will likely be UEFI shell-based tests, rather than host-based tests, but it's conceivable that some host-based examples may exist.

**Location**
: These should be checked in a "UnitTest" folder found within the folder containing the INF of the code being tested (module or library)

### Edk2- and UEFI-specific Codebase Analysis

_Mu_Build_ provides a framework for running static tests on the code base.  Simple tests like character encoding are examples.  In Project Mu we are working to expand this set of tests to include checking guids, checking for library classes, etc.

**Location**
: Can vary, as it uses the Plugin Model, so they are located anywhere within the code tree.  For Open Source common functionality tests we have added numerous to MU_Basecore/BaseTools/*PluginName*

### Basic Compilation

Each package must have a _*Pkg.ci.dsc_ file.  This DSC should list every module and library instance inf in the components section for the appropriate architectures.  This forces compilation when doing a CI build.  Since this is only to validate code builds successfully the library classes used to resolve dependencies should leverage null library instances whenever possible.  These null libs should minimize dependencies and make DSC management minimal. 

**Location**
: At the root of each package there should be a complete \<*PackageName*\>.ci.dsc file.  

### Compile-Time Asserts

Compile-time asserts can be used to check assertions in the code for build-time defined data.  An example could be confirming the size of a pixel array for an image matches the width x length.  

* C code (header and code)
  * Leverage C11 Static Assert feature for compile time verification in C code.
  * Read here for more details https://docs.microsoft.com/en-us/cpp/cpp/static-assert?view=vs-2019
  * And here https://en.cppreference.com/w/cpp/language/static_assert

Another great reason for these types of tests is that many IDEs will verify inline and show issues without the compiler.  

### Runtime Debug Asserts

Not really a "testing" tool but more of a debug and development practice.

> Describe more here about best practices and usage.  

### HBFA Based Unit Tests (INTEL)

> This section may not actually exist, and may entirely be the cmocka tests.

> Intel to describe more

### Host-Based Instrument Tests (INTEL)

> Intel to describe more

### Host-Based Fuzz Testing (INTEL)

> I don't know much about this except what Intel has published.

> * Read here for some intel information. https://firmware.intel.com/sites/default/files/Intel_UsingHBFAtoImprovePlatformResiliency.pdf
> * First iteration from Intel: https://github.com/tianocore/edk2-staging/tree/HBFA/HBFA 

### UEFI Shell-Based Functional Tests

Some tests are best run from within the UEFI environment.  These tests might be for APIs that leverage platform and global state.

> Review Project Mu Unit Test framework: https://github.com/Microsoft/mu_basecore/tree/release/201903/MsUnitTestPkg

### UEFI Shell-Based Audit Tests

These tests are run from UEFI to collect information in a machine-parsible format and then post-processed to compare against a "Golden Copy".  These tests often contain a UEFI shell application as well as a script for intelligent comparison against a known good "Golden Copy".  The "Golden Copy" could be device specific and is often curated and managed by a developer of that platform.

### SCT Framework

> Documented elsewhere.

### Shared Artifacts

Some testing formats -- especially the host-based unit tests, the fuzzing tests, and the shell-based functional tests -- may find it convenient to share logic in the form of libraries and helpers. These libraries may include mocks and stubs.

#### Shared Mocks (proposed definition)

A true mock is a functional interface that has almost no internal logic and is entirely scripted by the testing engine. An example of this might be a mocked version of GetVariable. When this mocked function is called by the business logic (the logic under test), all it does is ask the test what values it should return. Each test can script the order and content of the return values. As such, this is not an actual variable store, but can easily be used to force certain code paths.

#### Shared Stubs and Other Host Libs (proposed definition)

Stubs are a little more complicated than mocks. Rather than blindly returning values, stubs might have shorthand implementations behind them. In the example above, instead of GetVariable being entirely scripted, you might have an entire VariableServices interface (GetVariable, SetVariable, GetNextVariableName) that is backed by a simple array or other data structure. This stubbed version would behave very similarly to a real variable store, but can be pre-populated with specific contents prior to each test to demonstrate and excercise desired behaviors. Stubs and Mocks can also be combined in some instances to produce intricate behaviors.

## Where Should Things Live?

Code/Test                                   | Location 
---------                                   | --------
Host-Based Test for a library interface     | In the package that declares the library interface in its .DEC file. Should be located in the `*Pkg/Test/UnitTest/Library` directory.
Host-Based Test for a protocol interface    | In the package that declares the library interface in its .DEC file. Should be located in the `*Pkg/Test/UnitTest/Protocol` directory.
Host-Based Test for a library implementation   | In the directory that contains the library implementation, in a `UnitTest` subdirectory.
Host-Based Test for a function or feature   | In the either: the package that contains the feature code under the `*Pkg/Test/UnitTest/Functional` directory, or the `UefiHostUnitTestPkg/TestCases/Functional` directory if the feature spans multiple packages.
Compile-Time Asserts                        | These are placed inline in module code (H and C files)
Host-Based Fuzz Tests                       | Fuzz tests should follow a pattern similar to the Host-Based Unit Tests. They should live in the package most closely aligned with the interface or implementation being fuzzed in the `*Pkg/Test/FuzzTest` directory, optionally under `Library` or `Protocol` subdirectories, if it makes sense.
Shell-Based Test for a function or feature  | Should follow a pattern similar to the Host-Based Fuzz Tests. Directory should be `*Pkg/Test/ShellTest`.
Host-Based Library Implementations          | Host-Based Implementations of common libraries (eg. MemoryAllocationLibHost) should live in the same package that declares the library interface in its .DEC file in the `*Pkg/Library` directory. Should have 'Host' in the name.
Mocks and Stubs                             | Mock and Stub libraries should live in the `UefiHostTestPkg/Helpers` or the `UefiHostUnitTestPkg/Helpers` directory with either 'Mock' or 'Stub' in the library name.

## Testing Python

* Create pytest and/or python unit-test compatible tests.
* Make sure the python code passes the `flake8` "linter"

> Need to finish this documentation.

## RFC and Misc TODOs:

- CmockaHostUnitTestPkg, UefiHostTestPkg, and UefiHostUnitTestPkg should move to their own repo

- UefiHostFuzzTestCasePkg
  - Should move these into their packages as described for test cases and stubs.  
  - Common python in Seed should move into “edk2-pytools-library”
  - Seed files.  Are these test cases?  

- UefiHostCryptoPkg -  Should move necessary library instance into CryptoPkg

- UefiHostFuzzTestPkg - seems like it would go to edk2-test repo

- UefiHostTestTools - Move to edk2-test repo but should be called HBFATestTools?  

- UefiInstrumentTestCasePkg - Like the FuzzTestCasePkg?

- UefiInstrumentTestPkg - Like the FuzzTestPkg?  

- XmlSupportPkg
  - Move to edk2
  - Move UnitTestResultReportLibJunitFormat to UefiTestPkg



