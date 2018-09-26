/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_CONTAINER_CONVERTER_DETAIL_H_INCLUDED
#define TABLEIO_CONTAINER_CONVERTER_DETAIL_H_INCLUDED

/// @cond
namespace detail {
  
  template <class Converter_T, class booked_type, class is_i3converter>
  struct add_fields_impl {
    static void do_add(Converter_T &converter, I3TableRowDescriptionPtr desc, const booked_type &v)
    {
      converter.AddFields(desc, v);
    }
  };

  template <class Converter_T, class booked_type>
  struct add_fields_impl<Converter_T, booked_type, boost::true_type> {
    static void do_add(Converter_T &converter, I3TableRowDescriptionPtr desc, const booked_type &v)
    {
      *desc << *(converter.GetDescription(v));
    }
  };

  template <class Converter_T, class booked_type>
  inline void add_fields(Converter_T &converter, I3TableRowDescriptionPtr desc, const booked_type &v)
  {
    add_fields_impl< Converter_T, booked_type, typename boost::is_base_of<I3Converter, Converter_T>::type >::do_add(converter, desc, v);
  }


  template <class Converter_T, class booked_type, class is_i3converter>
  struct fill_single_row_impl {
    static void do_fill(Converter_T &converter, const booked_type &value, I3TableRowPtr rows, I3FramePtr)
    {
      converter.FillSingleRow(value, rows);
    }
  };

  template <class Converter_T, class booked_type>
  struct fill_single_row_impl<Converter_T, booked_type, boost::true_type> {
    static void do_fill(Converter_T &converter, const booked_type &value, I3TableRowPtr rows, I3FramePtr frame)
    {
      converter.Convert(value, rows, frame);
    }
  };

  template <class Converter_T, class booked_type>
  inline void fill_single_row(Converter_T &converter, const booked_type &value, I3TableRowPtr rows, I3FramePtr frame)
  {
    fill_single_row_impl< Converter_T, booked_type, typename boost::is_base_of<I3Converter, Converter_T>::type >::do_fill(converter, value, rows, frame);
  }

}
/// @endcond

#endif // TABLEIO_CONTAINER_CONVERTER_DETAIL_H_INCLUDED
