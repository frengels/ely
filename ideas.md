# reification

Reification is the process of transferring a value from the meta realm (compilation) to runtime.

Normally when a lisp such as racket has a quoted form live into the runtime realm it will simply provide the data as is from the compiler's data structures. Which means they won't have a static type. example `(quote 5)` would be a tagged union with the integer being active. Obviously when transferring this value to runtime it would be ideal to just have the integer without the tag left.

Other cases for reification exist with heap-allocated datastructures such as dynamic arrays. Allocations cannot be transferred from compile-time to runtime and with everything known at compile-time it would be ideal reify this data as a static array.
A potential issue with this is that the data cannot be resized anymore after reification, this is something to look into.

The proposed way to support reification would be through either adding the `#%reify` call or with a `#%literal` call, I'm leaning towards literal currently. Each type can register their own syntax transformer to handle their individual case.