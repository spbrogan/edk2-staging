# RFC TODOs:

- CmockaHostUnitTestPkg, UefiHostTestPkg, and UefiHostUnitTestPkg should move to their own repo
  - What is the logic behind the split between UefiHostTestPkg and UefiHostUnitTestPkg? Is there an assumption that UefiHostTestPkg contents could be used for things beyond UnitTesting?

- Document all file types and locations
  - Library Stubs
    - Host implementations should live alongside the Null implementation which should live in the package that defines the library
    - True stubs and shared mocks can live in the UnitTest package with "Mock" or "Stub" in the name.
  - Fuzz Tests
  - Functional Tests
    - UnitTest package with "Functional" in the name
  - Library interface tests
    - Should live in a package-level "Test" directory in the package that defines the interface
  - Library implementation tests
    - Should live in a UnitTest directory within the specific implementation
  - Protocol interface tests
    - Should live in a package-level "Test" directory in the package that defines the interface

# Unsorted TODOs Against HBFA

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


# Testing

Project Mu supports a few types of testing and this page will help provide some high level info and links for more information.  

## Types of Tests

### Edk2/UEFI specific code base analysis

*Mu_Build* provides a framework for running static tests on the code base.  Simple tests like character encoding are examples.  In Project Mu we are working to expand this set of tests to include checking guids, checking for library classes, etc.

* *LOCATION* can vary as it uses the Plugin Model so they are located anywhere within the code tree.  For Open Source common functionality tests we have added numerous to MU_Basecore/BaseTools/*PluginName*

### Basic compilation

Each package must have a _*Pkg.ci.dsc_ file.  This DSC should list every module and library instance inf in the components section for the appropriate architectures.  This forces compilation when doing a CI build.  Since this is only to validate code builds successfully the library classes used to resolve dependencies should leverage null library instances whenever possible.  These null libs should minimize dependencies and make DSC management minimal. 

* *LOCATION* At the root of each package there should be a complete \<*PackageName*\>.ci.dsc file.  

### Compile time asserts

Compile time asserts can be used to check assertions in the code for build time defined data.  An example could be confirming the size of a pixel array for an image matches the width x length.  

* C code (header and code)
  * Leverage C11 Static Assert feature for compile time verification in C code.
  * Read here for more details https://docs.microsoft.com/en-us/cpp/cpp/static-assert?view=vs-2019
  * And here https://en.cppreference.com/w/cpp/language/static_assert

Another great reason for these types of tests is that many IDEs will verify inline and show issues without the compiler.  

* *LOCATION* These changes go in your C and H code.  
  
### Runtime asserts

Not really a "testing" tool but more of a debug and development practice.  Describe more here about best practices and usage.  

### Host-Based Unit Tests (cmocka)

Host based unit tests let you compile your unit tests to run as an application in the host.  This method can also leverage "mocking" objects/functions so that unit tests can validate functionality in isolation and force unexpected edge cases.  These unit tests call c functions directly with known parameter sets to force good and bad conditions.  They are linked directly against C code in either a library instance or module. These tests leverage a UnitTest Library.  For Project Mu we have chosen cmocka.

* *LOCATION* These should be checked in a "UnitTest" folder found within the folder containing the INF of the code being tested (module or library)

### HBFA Based Unit Tests (INTEL)

These tests leverage the HBFA system to connect stubs and test cases.  
Intel to describe more

* *LOCATION* The stubs and test cases should reside in the package in which the stub interface or test case target API is defined.  
  * Common Stubs: \*Pkg/Test/HBFA/Stub
  * Test Cases: \*Pkg/Test/HBFA/TestCases

### Host-Based Instrument Tests (INTEL)

### Host-Based Fuzz Testing (INTEL)

I don't know much about this except what Intel has published.

* Read here for some intel information. https://firmware.intel.com/sites/default/files/Intel_UsingHBFAtoImprovePlatformResiliency.pdf
* First iteration from Intel: https://github.com/tianocore/edk2-staging/tree/HBFA/HBFA 

* *LOCATION* The fuzz test cases should reside in the package in which the test case target API is defined.  
  * Test Cases: \*Pkg/Test/HBFA/FuzzTestCases

### UEFI Shell Based Functional Tests

Some tests are best run from within the UEFI environment.  These tests might be for APIs that leverage platform and global state.

* Review Project Mu Unit Test framework: https://github.com/Microsoft/mu_basecore/tree/release/201903/MsUnitTestPkg

### UEFI Shell Based Audit Tests

These tests are run from UEFI to collect information in a machine parsable format and then post processed to compare against a "Golden Copy".  These tests often contain a UEFI shell application as well as a script for intelligent comparison against a known good "Golden Copy".  The "Golden Copy" could be device specific and is often curated and managed by a developer of that platform.

### SCT Test cases

Todo: collect more info

## Test Infrastructure

### HBFA

### Host Based Unit Tests

### UEFI Shell Based Unit Tests

### SCT Framework

## Testing Python

* Create pytest and/or python unit-test compatible tests.
* Make sure the python code passes the `flake8` "linter"
