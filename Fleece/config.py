excluded = ["FLStr","operatorslice","operatorFLSlice"]
default_param_name = {"FLValue":"value","FLSliceResult":"slice","FLSlice":"slice","FLArray":"array","FLArrayIterator*":"i","FLDictIterator*":"i","FLDict":"dict","FLDictKey":"key","FLKeyPath":"keyPath","FLDictKey*":"dictKey","FLSharedKeys":"shared","FLEncoder":"encoder","long":"l","ulong":"u","bool":"b","float":"f","double":"d","FLError*":"outError","int64_t":"l","uint64_t":"u","FLString":"str","FLStringResult":"str"}
param_bridge_types = ["FLSlice", "FLString", "size_t", "size_t*"]
force_no_bridge = ["FLSlice_Compare", "FLSlice_Equal"]
return_bridge_types = ["FLSliceResult", "FLSlice", "size_t", "FLString", "FLStringResult"]
type_map = {"int32_t":"int","uint32_t":"uint","int64_t":"long","uint64_t":"ulong","size_t":"UIntPtr","size_t*":"UIntPtr*","unsigned":"uint","FLValue":"FLValue*","FLDict":"FLDict*","FLArray":"FLArray*","FLEncoder":"FLEncoder*","FLSharedKeys":"FLSharedKeys*","FLKeyPath":"FLKeyPath*"}
literals = {"FLSlice_Compare":".nobridge .int FLSlice_Compare FLSlice:left FLSlice:right"}
reserved = ["string"]
