# libbuild2-hello

Test build system module for `build2`.

There are two ways to set things up: using only the build system, which works
best if you just want to kick the tires, or using the project manager, which
is how you would do it if you were developing a real module and needed the
ability to CI, manage and publish releases, etc. The latter applies to other
`build2` modules which often refer to these setup instructions instead of
repeating them.


## Using only the build system

```
git clone .../libbuild2-hello.git
cd libbuild2-hello/
b configure: libbuild2-hello/ config.config.load=~build2
b configure: libbuild2-hello-tests/ config.import.libbuild2_hello=libbuild2-hello/
b test
```

> If building the module in a cross-compile configuration (normally together
> with the build system), then we need to manually suppress importing of the
> host build system by additing `import.build2=[null]` to module's
> `config.build`.


## Using the project manager

Because the module and the tests have to use different build configurations,
the initialization is more elaborate compared to a typical project:

```
git clone .../libbuild2-hello.git
cd libbuild2-hello/

bdep init --empty

bdep config create --no-default --forward @mod ../libbuild2-hello-build/module/ cc config.config.load=~build2
bdep init @mod -d libbuild2-hello/
b libbuild2-hello/

bdep config create --default --forward @test ../libbuild2-hello-build/tests/ cc config.import.libbuild2_hello=../libbuild2-hello-build/module/
bdep init @test -d libbuild2-hello-tests/ --no-sync
bdep sync
```

Once this is done, we can develop using `bdep` or the build system as usual:

```
bdep test                       # run tests in libbuild2-hello-tests/
b test                          # the same
b libbuild2-hello/              # update the module directly
```

We can also CI our module, manage releases, and publish it to the package
repository:

```
bdep ci        # submits only the module (which pulls in the tests)

bdep release   # releases both the module and the tests

bdep publish   # submits both the module and the tests
```

Note that the `bdep-ci` and `bdep-publish` commands are tweaked to do the
right thing in this setup with the default options files located in the
`.build2/` subdirectory.


## Using in other projects during development

The above setup makes sure the module can be found by its own tests, examples,
etc. When developing a real module, however, we often want to use our
development version in other projects that use this module. While explicitly
configuring each such project with `config.import.libbuild2_*` is possible, it
is also tedious. Instead, we can specify the necessary variable as a global
override in our user-wide `~/.build2/b.options` options file:

```
!config.import.libbuild2_hello=/home/.../libbuild2-hello-build/module/
```


## Developing modules that require bootstrapping

Developing a module that requires bootstrapping (that is, which must be loaded
in the project's `bootstrap.build` rather than `root.build`) is more
complicated. The main issue is that when such a module has to be loaded, the
project's configuration is not yet available. Specifically, neither non-global
variable overrides have been entered nor has the `config.build` file been
loaded. As a result, the `config.import.libbuild2_*` variable specified while
creating the `@test` configuration in the steps above has no effect.

The recommended approach to developing such modules is to use the global
variable override in our user-wide `~/.build2/b.options` options file, the
same as in the previous section:

```
!config.import.libbuild2_hello=/home/.../libbuild2-hello-build/module/
```

This global override should be added before creating the `@test`
configuration.

If you are planning to CI such a module, then it must be explicitly marked as
requiring bootstrap by adding the following requirement to its `manifest`:

```
requires: bootstrap
```

Note also that loading of such a module may be omitted with the
`--no-external-modules` build system option provided the project itself is
being only bootstrapped. This option, for example, is used by the CI
infrastructure when preparing the package distribution in the bootstrap mode
(happens when the package is checked out from a version control-based
repository) or when querying for information with the `info` meta-operation.
As a result, a module that requires bootstrapping should be designed to
tolerate such an eventuality. Generally, a module should strive to perform
work during bootstrap that can only be performed during bootstrap, leaving
everything else to the initialization step.
