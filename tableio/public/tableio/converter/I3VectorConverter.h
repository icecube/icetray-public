/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#ifndef TABLEIO_I3VECTORCONVERTER_H_INCLUDED
#define TABLEIO_I3VECTORCONVERTER_H_INCLUDED

#include <tableio/I3Converter.h>
#include <dataclasses/I3Vector.h>
#include <tableio/converter/container_converter_detail.h>


template <class converter_type,
	  typename value_type = typename converter_type::booked_type >
class I3VectorConverter : public I3ConverterImplementation< I3Vector< value_type > > {
private:
  typedef I3Vector< value_type > vector_type;

  size_t GetNumberOfRows(const vector_type &v)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    return v.size();
  }

  I3TableRowDescriptionPtr CreateDescription(const vector_type &v)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    I3TableRowDescriptionPtr desc = 
      I3TableRowDescriptionPtr(new I3TableRowDescription() );
    desc->isMultiRow_ = true;
    desc->AddField<tableio_size_t>("vector_index", "", "index in vector");

    if (v.size())
      detail::add_fields(converter_, desc, v.front());
    else
      detail::add_fields(converter_, desc, value_type());

    return desc;
  }

  size_t FillRows(const vector_type &v, I3TableRowPtr rows)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    size_t row = rows->GetCurrentRow();
    for(typename vector_type::const_iterator iter = v.begin();
	iter != v.end(); iter++)
      {
	rows->SetCurrentRow(row);
	rows->Set<tableio_size_t>("vector_index", row);
	
	detail::fill_single_row(converter_, *iter, rows, this->currentFrame_);
	
	++row;
      } // loop over vector

    return v.size();
  }

  converter_type converter_;
};

#endif // TABLEIO_I3VECTORCONVERTER_H_INCLUDED
