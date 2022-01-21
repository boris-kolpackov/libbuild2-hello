#include <libbuild2/scope.hxx>
#include <libbuild2/target.hxx>
#include <libbuild2/function.hxx>
#include <libbuild2/variable.hxx>
#include <libbuild2/algorithm.hxx>
#include <libbuild2/diagnostics.hxx>

#include <libbuild2/functions-name.hxx> // to_target()

#include <libbuild2/bin/target.hxx>

namespace build2
{
  namespace hello
  {
    void
    functions (function_map& m)
    {
      function_family f (m, "hello");

      // Resolve target name to target. @@ TODO
      //
      // Note that this function is not pure.
      //
      f.insert (".resolve", false) += [] (const scope* s, names ns)
      {
        if (s == nullptr)
          fail << "hello.resolve() called out of scope" << endf;

        if (ns.empty () || ns.size () != (ns[0].pair ? 2 : 1))
          fail << "hello.resolve() expects single target";

        context& ctx (s->ctx);

        const target& et (to_target (*s, move (ns)));

        if (!et.is_a<exe> ())
          fail << "hello.resolve() expects an exe{} target";

        target& ot (
          search_new_locked (
            ctx,
            bin::obje::static_type,
            s->out_path (), // @@ Not an accurate assumption.
            dir_path (),    // Always in out.
            et.name,
            nullptr,
            s).first);

        ot.append (*ctx.var_pool.find ("cxx.poptions")) += strings {"-DFOO"};
      };
    }
  }
}
