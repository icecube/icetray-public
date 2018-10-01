MACRO(UNIQUE var_name list)
  # Make the given list have only one instance of each unique element and
  # store it in var_name.
  SET(unique_tmp "")
  FOREACH(l ${list})
    IF(NOT "${unique_tmp}" MATCHES "(^|;)${l}(;|$)")
      SET(unique_tmp ${unique_tmp} ${l})
    ENDIF(NOT "${unique_tmp}" MATCHES "(^|;)${l}(;|$)")
  ENDFOREACH(l)
  SET(${var_name} ${unique_tmp})
ENDMACRO(UNIQUE)
