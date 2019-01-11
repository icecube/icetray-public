################################################################################
# Functions for list operations.
################################################################################

MACRO(SORT var_name list)
  # Sort the given list and store it in var_name.
  SET(sort_tmp1 "")
  FOREACH(l ${list})
    SET(sort_inserted 0)
    SET(sort_tmp2 "")
    FOREACH(l1 ${sort_tmp1})
      IF("${l}" STRLESS "${l1}" AND ${sort_inserted} EQUAL 0)
        SET(sort_tmp2 ${sort_tmp2} "${l}" "${l1}")
        SET(sort_inserted 1)
      ELSE("${l}" STRLESS "${l1}" AND ${sort_inserted} EQUAL 0)
        SET(sort_tmp2 ${sort_tmp2} "${l1}")
      ENDIF("${l}" STRLESS "${l1}" AND ${sort_inserted} EQUAL 0)
    ENDFOREACH(l1)
    IF(${sort_inserted} EQUAL 0)
      SET(sort_tmp1 ${sort_tmp1} "${l}")
    ELSE(${sort_inserted} EQUAL 0)
      SET(sort_tmp1 ${sort_tmp2})
    ENDIF(${sort_inserted} EQUAL 0)
  ENDFOREACH(l)
  SET(${var_name} ${sort_tmp1})
ENDMACRO(SORT)

