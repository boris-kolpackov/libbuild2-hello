#include <libbuild2/hello/init.hxx>

#include <libbuild2/scope.hxx>
#include <libbuild2/function.hxx>
#include <libbuild2/diagnostics.hxx>

using namespace std;

namespace build2
{
  namespace hello
  {
    void
    functions (function_map&); // functions.cxx

    bool
    init (scope& rs,
          scope&,
          const location& l,
          bool,
          bool,
          module_init_extra&)
    {
      context& ctx (rs.ctx);

      // Register the module function family if this is the first instance of
      // this modules.
      //
      if (!function_family::defined (ctx.functions, "hello"))
        functions (ctx.functions);

      info (l) << "module hello initialized";
      return true;
    }

    static const module_functions mod_functions[] =
    {
      {"hello", nullptr, init},
      {nullptr, nullptr, nullptr}
    };

    const module_functions*
    build2_hello_load ()
    {
      return mod_functions;
    }
  }
}
