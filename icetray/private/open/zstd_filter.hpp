#ifndef ZSTD_FILTER_HPP
#define ZSTD_FILTER_HPP

#include <boost/iostreams/filtering_stream.hpp>
#include <zstd.h>
#include <algorithm>

class zstd_compressor : public boost::iostreams::multichar_output_filter{
public:
	using char_type=char;
	
	zstd_compressor(int compressionLevel):
	cstream(nullptr,stream_delete),
	compressionLevel(compressionLevel),
	streamInitialized(false),
	ibufSize(0),ibufUsed(0)
	{}
	
	//boost::iostreams really likes to copy when it should move. This filter
	//cannot generally be copied, but doing so is possible between construction
	//and first real use, which in practice is the only time it's needed.
	zstd_compressor(const zstd_compressor& other):
	cstream(nullptr,stream_delete),
	compressionLevel(other.compressionLevel),
	streamInitialized(other.streamInitialized),
	ibufSize(other.ibufSize),ibufUsed(other.ibufUsed)
	{
		assert(!other.streamInitialized);
		assert(!other.cstream);
		assert(!other.ibuf);
		assert(!other.obuf);
	}
	
	zstd_compressor(zstd_compressor&& other):
	cstream(std::move(other.cstream)),
	compressionLevel(other.compressionLevel),
	streamInitialized(other.streamInitialized),
	ibuf(std::move(other.ibuf)),
	obuf(std::move(other.obuf)),
	ibufSize(other.ibufSize),ibufUsed(other.ibufUsed)
	{
		other.streamInitialized=false;
		other.ibufSize=0;
		other.ibufUsed=0;
	}
	
	~zstd_compressor()=default;
	zstd_compressor& operator=(const zstd_compressor&)=delete;
	zstd_compressor& operator=(zstd_compressor&& other){
		if(&other!=this){
			cstream=std::move(other.cstream);
			compressionLevel=other.compressionLevel;
			streamInitialized=other.streamInitialized;
			ibuf=std::move(other.ibuf);
			obuf=std::move(other.obuf);
			ibufSize=other.ibufSize;
			ibufUsed=other.ibufUsed;
			
			other.streamInitialized=false;
			other.ibufSize=0;
			other.ibufUsed=0;
		}
		return(*this);
	}
	
	//compress n bytes of input from src to dest
	template<typename Sink>
	std::streamsize write(Sink& dest, const char* src, std::streamsize n){
		if(!streamInitialized)
			initStream();
		
		std::streamsize result=n;
		//Copy as much input into the input buffer as possible,
		//run the compression each time the input buffer is filled,
		//and then insert the remainder.
		//In practice, n tends to be _much_ smaller than ibufSize.
		while(n>0){
			std::streamsize to_copy=std::min(n,std::streamsize(ibufSize-ibufUsed));
			memcpy(ibuf.get()+ibufUsed,src,to_copy);
			n-=to_copy;
			ibufUsed+=to_copy;
			if(ibufUsed<ibufSize)
				break;
			ZSTD_inBuffer input{ibuf.get(),ibufSize,0};
			while(input.pos<input.size){
				ZSTD_outBuffer output{obuf.get(),ZSTD_CStreamOutSize(),0};
				size_t result=ZSTD_compressStream(cstream.get(),&output,&input);
				if(ZSTD_isError(result))
					log_fatal_stream("ZSTD_compressStream() error: " << ZSTD_getErrorName(result));
				boost::iostreams::write(dest,obuf.get(),output.pos);
			}
			ibufUsed=0;
		}
		return(result);
	}
	
	//flush buffered input
	template<typename Sink>
	void close(Sink& dest){
		//If no data was compressed initialize the stream anyway, so that we can
		//finalize it and end up with a valid file.
		if(!streamInitialized)
			initStream();
		if(ibufUsed){
			ZSTD_inBuffer input{ibuf.get(),ibufUsed,0};
			while(input.pos<input.size){
				ZSTD_outBuffer output{obuf.get(),ZSTD_CStreamOutSize(),0};
				size_t err=ZSTD_compressStream(cstream.get(),&output,&input);
				if(ZSTD_isError(err))
					log_fatal_stream("ZSTD_compressStream error: " << ZSTD_getErrorName(err));
				boost::iostreams::write(dest,obuf.get(),output.pos);
			}
			ibufUsed=0;
		}
		size_t bytes_remaining;
		do{
			ZSTD_outBuffer output{obuf.get(),ZSTD_CStreamOutSize(),0};
			bytes_remaining=ZSTD_endStream(cstream.get(),&output);
			boost::iostreams::write(dest,obuf.get(),output.pos);
		}while(bytes_remaining>0);
	}
private:
	std::unique_ptr<ZSTD_CStream,void(*)(ZSTD_CStream*)> cstream;
	int compressionLevel;
	bool streamInitialized;
	std::unique_ptr<char_type[]> ibuf;
	std::unique_ptr<char_type[]> obuf;
	std::size_t ibufSize, ibufUsed;
	
	static void stream_delete(ZSTD_CStream* stream){
		if(stream)
			ZSTD_freeCStream(stream);
	}
	
	void initStream(){
		cstream.reset(ZSTD_createCStream());
		ZSTD_initCStream(cstream.get(),compressionLevel);
		ibufSize=ZSTD_CStreamInSize();
		ibufUsed=0;
		ibuf.reset(new char_type[ibufSize]);
		obuf.reset(new char_type[ZSTD_CStreamOutSize()]);
		streamInitialized=true;
	}
};

class zstd_decompressor : public boost::iostreams::multichar_input_filter{
public:
	using char_type=char;
	
	zstd_decompressor():
	dstream(nullptr,stream_delete),
	streamInitialized(false),
	ibufSize(0),
	inputEnd(false)
	{}
	
	//boost::iostreams really likes to copy when it should move. This filter
	//cannot generally be copied, but doing so is possible between construction
	//and first real use, which in practice is the only time it's needed.
	zstd_decompressor(const zstd_decompressor& other):
	dstream(nullptr,stream_delete),
	streamInitialized(other.streamInitialized),
	ibufSize(other.ibufSize),
	inputEnd(other.inputEnd)
	{
		assert(!other.streamInitialized);
		assert(!other.dstream);
		assert(!other.ibuf);
	}
	
	zstd_decompressor(zstd_decompressor&& other):
	dstream(std::move(other.dstream)),
	streamInitialized(other.streamInitialized),
	ibuf(std::move(other.ibuf)),
	ibufSize(other.ibufSize),
	zibuf(other.zibuf),
	inputEnd(other.inputEnd)
	{
		other.streamInitialized=false;
		other.ibufSize=0;
		other.zibuf=ZSTD_inBuffer{nullptr,0,0};
		other.inputEnd=false;
	}
	
	~zstd_decompressor()=default;
	zstd_decompressor& operator=(const zstd_decompressor&)=delete;
	zstd_decompressor& operator=(zstd_decompressor&& other){
		if(&other!=this){
			dstream=std::move(other.dstream);
			streamInitialized=other.streamInitialized;
			ibuf=std::move(other.ibuf);
			ibufSize=other.ibufSize;
			zibuf=other.zibuf;
			inputEnd=other.inputEnd;
			
			other.streamInitialized=false;
			other.ibufSize=0;
			other.zibuf=ZSTD_inBuffer{nullptr,0,0};
			other.inputEnd=false;
		}
		return(*this);
	}
	
	//read as much input from src as necessary to write n bytes of decompressed
	//data to dest
	template <typename Source>
	std::streamsize read(Source& src, char_type* dest, std::streamsize n){
		if(!streamInitialized)
			initStream();
		
		std::streamsize result=0;
		
		//First, see if we have anything sitting in ibuf which still needs to be
		//decompressed
		while(zibuf.pos<zibuf.size && n>0){
			ZSTD_outBuffer zobuf{dest,(size_t)n,0};
			size_t err=ZSTD_decompressStream(dstream.get(), &zobuf , &zibuf);
			if(ZSTD_isError(err))
				log_fatal_stream("ZSTD_decompressStream error: " << ZSTD_getErrorName(err));
			std::streamsize output_size=zobuf.pos;
			dest+=output_size;
			n-=output_size;
			result+=output_size;
		}
		
		//If that didn't produce enough data, we need to actually fetch more from
		//src, insert it into ibuf, and decompress.
		while(n>0 && !inputEnd){
			//first, try to grab enough data to fill ibuf
			zibuf.size=0;
			zibuf.pos=0;
			while(zibuf.size<ibufSize){
				int c=boost::iostreams::get(src);
				if(c==EOF){
					inputEnd=true;
					break;
				}
				ibuf[zibuf.size++]=c;
			}
			//then try to decompress data until enough output is produced
			while(zibuf.pos<zibuf.size && n>0){
				ZSTD_outBuffer zobuf{dest,(size_t)n,0};
				size_t err=ZSTD_decompressStream(dstream.get(), &zobuf , &zibuf);
				if(ZSTD_isError(err))
					log_fatal_stream("ZSTD_decompressStream error: " << ZSTD_getErrorName(err));
				std::streamsize output_size=zobuf.pos;
				dest+=output_size;
				n-=output_size;
				result+=output_size;
			}
		}
		
		return(result);
	}
	
	template <typename Source>
	void close(Source& src){/*Do nothing*/}
private:
	std::unique_ptr<ZSTD_DStream,void(*)(ZSTD_DStream*)> dstream;
	bool streamInitialized;
	std::unique_ptr<char_type[]> ibuf;
	std::size_t ibufSize;
	ZSTD_inBuffer zibuf;
	bool inputEnd;
	
	static void stream_delete(ZSTD_DStream* stream){
		if(stream)
			ZSTD_freeDStream(stream);
	}
	
	void initStream(){
		dstream.reset(ZSTD_createDStream());
		ZSTD_initDStream(dstream.get());
		ibufSize=ZSTD_DStreamInSize();
		ibuf.reset(new char_type[ibufSize]);
		zibuf=ZSTD_inBuffer{ibuf.get(),0,0};
		streamInitialized=true;
	}
};

#endif //ZSTD_FILTER_HPP
