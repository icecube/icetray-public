#include <I3Test.h>

#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>

#include <tableio/I3Converter.h>
#include <tableio/I3TableWriter.h>
#include <tableio/textwriter/I3CSVTableService.h>
#include <tableio/converter/pybindings.h>

//There are two important cases related to inheritance which we need to get right.
//First, we may have a derived class which has its own more specific converter,
//which should override the more generic converter associated with the base class.
//Second, we may have a derived class which does not have a more specific
//converter, and so should be handled by the generic base class converter.

class Foo : public I3FrameObject{
public:
	Foo():a(0){}
	Foo(int a):a(a){}
	void SetA(int a_){ a=a_; }
	virtual int GetA() const{ return(a); }
protected:
	int a;
};

//a Foo which for whatever reason scales its contents by 2
class Foo2 : public Foo{
public:
	Foo2(){}
	Foo2(int a):Foo(a){}
	virtual int GetA() const{ return(2*a); }
};

//a Foo with an extra field
class Bar : public Foo{
public:
	Bar(){}
	Bar(int a, int b):Foo(a),b(b){}
	void SetB(int b_){ b=b_; }
	int GetB() const{ return(b); }
private:
	int b;
};

class FooConverter :
public I3ConverterImplementation<Foo>,
public boost::enable_shared_from_this<FooConverter>{
public:
	virtual I3TableRowDescriptionPtr CreateDescription(const Foo& f){
		I3TableRowDescriptionPtr desc(new I3TableRowDescription());
		desc->AddField<int>("a", "", "Parameter A");
		return(desc);
	}
	
	virtual size_t GetNumberOfRows(const Foo& f){ return(1); }
	
	virtual size_t FillRows(const Foo& f, I3TableRowPtr rows){
		rows->Set<int>("a",f.GetA());
		return(1);
	}
	
	boost::python::object operator()(){
        return(boost::python::object(shared_from_this()));
    }
};

class BarConverter :
public I3ConverterImplementation<Bar>,
public boost::enable_shared_from_this<BarConverter>{
public:
	virtual I3TableRowDescriptionPtr CreateDescription(const Bar& b){
		I3TableRowDescriptionPtr desc(new I3TableRowDescription());
		desc->AddField<int>("a", "", "Parameter A");
		desc->AddField<int>("b", "", "Parameter B");
		return(desc);
	}
	
	virtual size_t GetNumberOfRows(const Bar& b){ return(1); }
	
	virtual size_t FillRows(const Bar& b, I3TableRowPtr rows){
		rows->Set<int>("a",b.GetA());
		rows->Set<int>("b",b.GetB());
		return(1);
	}
	
	boost::python::object operator()(){
        return(boost::python::object(shared_from_this()));
    }
};

TEST_GROUP(InheritanceConverterTests);

TEST(ConvertersAcceptSubclasses){
	boost::shared_ptr<Foo> f(new Foo(11));
	boost::shared_ptr<Foo2> f2(new Foo2(11));
	boost::shared_ptr<Bar> b(new Bar(5,6));
	
	FooConverter fconverter;
	ENSURE_EQUAL(fconverter.CanConvert(f),I3Converter::ExactConversion);
	ENSURE_EQUAL(fconverter.CanConvert(f2),I3Converter::InexactConversion);
	ENSURE_EQUAL(fconverter.CanConvert(b),I3Converter::InexactConversion);
	
	BarConverter bconverter;
	ENSURE_EQUAL(bconverter.CanConvert(f),I3Converter::NoConversion);
	ENSURE_EQUAL(bconverter.CanConvert(f2),I3Converter::NoConversion);
	ENSURE_EQUAL(bconverter.CanConvert(b),I3Converter::ExactConversion);
}

struct I3TableWriterTestAccess{
	static boost::shared_ptr<I3Converter>
	FindConverter(I3TableWriter& tw, boost::shared_ptr<const I3FrameObject> obj){
		return(tw.FindConverter(obj));
	}
};

TEST(MostSpecificConverterIsSelected){
	//build our collection of converters
	{ //many hoops. much jumping.
		using boost::python::bases;
		boost::python::import("icecube.tableio");
		boost::python::class_<Foo, bases<I3FrameObject>, boost::shared_ptr<Foo> >
		("Foo",boost::python::no_init);
		boost::python::class_<Foo2, bases<I3FrameObject>, boost::shared_ptr<Foo2> >
		("Foo2",boost::python::no_init);
		boost::python::class_<Bar, bases<I3FrameObject>, boost::shared_ptr<Bar> >
		("Bar",boost::python::no_init);
		I3CONVERTER_NAMESPACE("tableio");
		I3CONVERTER_EXPORT_DEFAULT(FooConverter,"")
		.def("__call__",&FooConverter::operator());
		I3CONVERTER_EXPORT_DEFAULT(BarConverter,"")
		.def("__call__",&BarConverter::operator());
	}
	boost::shared_ptr<FooConverter> fconverter(new FooConverter);
	boost::shared_ptr<BarConverter> bconverter(new BarConverter);
	std::vector<I3ConverterMillPtr> converters;
	converters.push_back(boost::make_shared<I3ConverterMill>(boost::python::object(fconverter)));
	converters.push_back(boost::make_shared<I3ConverterMill>(boost::python::object(bconverter)));
	
	boost::shared_ptr<I3CSVTableService> ts(new I3CSVTableService("dummy_directory"));
	std::vector<std::string> streams;
	I3TableWriter tw(ts, converters, streams);
	
	boost::shared_ptr<Foo> f(new Foo(11));
	boost::shared_ptr<Foo2> f2(new Foo2(11));
	boost::shared_ptr<Bar> b(new Bar(5,6));
	
	//check that each object gets picked up by the right converter
	ENSURE(I3TableWriterTestAccess::FindConverter(tw,f)==fconverter);
	ENSURE(I3TableWriterTestAccess::FindConverter(tw,f2)==fconverter);
	ENSURE(I3TableWriterTestAccess::FindConverter(tw,b)==bconverter);

	namespace fs = boost::filesystem;
	fs::remove_all("dummy_directory");
}
