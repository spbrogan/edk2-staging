# CI and PR Gates

## Background

Historically, while the TianoCore maintainers and stewards have done a fantastic job of keeping contribution policies consistent and contributions clean and well-documented, there have been few processes that ran to verify the sanity, cleanliness, and efficacy of the codebase, and even fewer that publicly published their results for the community at large. As such, the codebase is rife with cross-dependencies and modules that don't build on all supported toolchains, just to name a couple issues.

Adding continuous integration (and potentially PR gates) to the checkin process ensures that simple errors like these are caught and can be fixed on a regular basis.

## Strategy

While a number of CI solutions exist, this proposal will focus on the usage of Azure Dev Ops and Build Pipelines.

[Results](https://dev.azure.com/tianocore/edk2-ci-play/_build?definitionId=12)

For any of the tests, there can be codebase- and package-level configuration of tests that need to be skipped or modified for special considerations. For example, if a particular library is known to fail building with a specific toolchain/architecture combination (or if a given module is generally known to not be buildable without additional effort) the Code Compilation test can be skipped for this configuration.

## CI Test Types

### Module Inclusion Test

This test scans all available modules (via INF files) and compares them to the package-level DSC file for the package each module is contained within. The test considers it an error if any module does not appear in the `Components` section of at least one package-level DSC (indicating that it would not be built if the package were built).

### Code Compilation Test

Once the Module Inclusion Test has verified that all modules would be built if all package-level DSCs were built, the Code Compilation Test simply runs through and builds every package-level DSC on every toolchain and for every architecture that is supported. Any module that fails to build is considered an error.

### Host-Based UnitTests

The [Testing RFC doc](Readme-Testing-RFC.md) has much more detail on this, but the basic idea is that host-based unit tests can be compiled against individual modules and libraries and run on the build agent (in this case, the Dev Ops build server). The successful and failing test case results are collected and included in the final build report.