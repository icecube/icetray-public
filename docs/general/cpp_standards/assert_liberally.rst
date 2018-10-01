
Assert liberally to document internal assumptions and invariants
----------------------------------------------------------------

Be assertive!  Use **assert** or an equivalent liberally to document 
assumptions internal to a module (i.e., where the caller and callee
are maintained by the same person or team) that must always be true
and otherwise represent programming errors (e.g. violations of a 
function's postconditions detected by the caller of the function).
Ensure that assertions don't perform side effects.
