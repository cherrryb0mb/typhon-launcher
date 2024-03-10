/*
 *  p3t.c
 *  
 *
 *  Created by Johannes Wilke on 2008/08/16.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "p3t.h"
#include "gim.h"

bool extracthd=1;
bool extractsd=1;
bool extractgim=1;
bool extractvag=1;
int p3tcolor=0;

int hdcount=0;
int sdcount=0;
int gimcount=0;
int vagcount=0;



// GIM

long uncompressGIMBuffer(void* compressed_buffer, size_t compressed_buffer_size, void** gim_buffer)
{
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  
  size_t gim_buffer_size = compressed_buffer_size * 2;
  
  while(1)
  {
    strm.next_in = (Bytef*)compressed_buffer;
    strm.avail_in = compressed_buffer_size;  

    long inflate_init_result = inflateInit (&strm);
    if (inflate_init_result != Z_OK)
    {
      *gim_buffer = 0;
      return -GIM_CANNOT_INITIALIZE;
    }
  
    *gim_buffer = (void*)new Bytef[gim_buffer_size];
    
    strm.next_in = (Bytef*)compressed_buffer;
    strm.avail_in = compressed_buffer_size;
    strm.total_in = 0;
    strm.next_out = (Bytef*)*gim_buffer;
    strm.avail_out = gim_buffer_size;
    strm.total_out = 0;
    
    int inflate_result = inflate(&strm, Z_FINISH);
	if (inflate_result == Z_STREAM_END) break;
	else 
	{
		if (inflate_result == Z_BUF_ERROR) 
		{
			delete[](Bytef*)*gim_buffer;
			gim_buffer_size *= 2;
			inflateEnd(&strm);
#ifdef OUTPUT_DEBUG_TEXT
			printf("GIM buffer too slow, increasing to: %i\n",gim_buffer_size);
#endif
		}
		else
		{
			printf("error in zstream: %i\n",inflate_result);
			return inflate_result;
		}
	}
  }  
  
  inflateEnd(&strm);
  
  return strm.total_out;
}

// GIM END

typedef unsigned int p3tOffsetType;		// 4 byte
typedef unsigned int p3tSizeType;		// 4 byte

struct p3tLocationEntry 				// 8 byte
{
  p3tOffsetType offset;					// 4 byte
  p3tSizeType size;						// 4 byte
};

struct p3tHeader						//72 byte
{
	unsigned int magic; 				// 4 byte
	unsigned int version;				// 4 byte
	struct p3tLocationEntry elements;	// 8 byte
	struct p3tLocationEntry ids;		// 8 byte
	struct p3tLocationEntry strings;	// 8 byte
	struct p3tLocationEntry integers;	// 8 byte
	struct p3tLocationEntry decimals;	// 8 byte
	struct p3tLocationEntry files;		// 8 byte
	unsigned int padding[4];			//16 byte
};

union p3tAttributeValue					// 8 byte
{
  p3tLocationEntry location;			// 8 byte
  p3tInt integer;						// 4 byte
  p3tFloat decimal;						// 4 byte
};

typedef unsigned int p3tAttributeType;	// 4 byte

struct p3tAttributeHeader				//16 byte
{
  p3tOffsetType name_offset;			// 4 byte
  p3tAttributeType type;				// 4 byte
  p3tAttributeValue value;				// 8 byte
};

typedef enum {
  P3T_GIM_FILE,							// 8 byte
  P3T_JPG_FILE,							// 8 byte
  P3T_VAG_FILE							// 8 byte
} p3tFileType;							// 8 byte

struct p3tAttribute						//36 byte
{
  p3tAttributeHeader header;			//16 byte
  unsigned char* file_data;				// 8 byte
  p3tSizeType file_size;				// 4 byte
  p3tFileType file_type; 				// 8 byte
};

typedef std::list<p3tAttribute> p3tAttributeList;// 16 byte

struct p3tElementHeader					//7*4=28 byte
{
  p3tOffsetType name_offset;
  unsigned int no_attributes;
  p3tOffsetType parent_offset;
  p3tOffsetType previous_sibling_offset;
  p3tOffsetType next_sibling_offset;
  p3tOffsetType first_child_offset;
  p3tOffsetType last_child_offset;
};

struct p3tElement						//48 byte
{
  p3tElementHeader header;				//28 byte
  p3tAttributeList attributes;			//16 byte
  
  p3tOffsetType offset;					// 4 byte
};
typedef std::map<p3tOffsetType, p3tElement> elementMap;

typedef std::map<p3tOffsetType, p3tString> stringMap;
typedef std::map<p3tOffsetType, p3tId> idMap;
typedef std::map<p3tOffsetType, p3tOffsetType> idParentOffsetMap;

// ================================ Ripping p3t FILE =========================

int loadP3THeader(FILE* p3t_file_handle, p3tHeader& p3theader)
{
// header debugging, maybe useful later
/*
	char m[4];
	size_t s =fread(m,1,4,p3t_file_handle);
	
	printf("read: %X %X %X %X\n",m[0],m[1],m[2],m[3]);
	printf("my magic: %s\n",m);
  	//fseek(p3t_file_handle,4,SEEK_SET);

	char* v = (char*) malloc(sizeof(char)*4);
  	s =fread(v,1,4,p3t_file_handle);
	//printf("red: %X %X %X %X\n",m[0],m[1],m[2],m[3]);
	long i = (long) v;
	printf("my version: %i\n", i);
*/
	
  p3tSizeType no_header_bytes_to_read = sizeof(p3theader);
  p3tSizeType no_header_bytes_read = fread(&p3theader, 1, no_header_bytes_to_read, p3t_file_handle);
  if (no_header_bytes_read != no_header_bytes_to_read)
  {
    fclose(p3t_file_handle);
    return P3T_COULD_NOT_READ_HEADER;
  }
  
	p3theader.magic = ntohl(p3theader.magic);
	p3theader.version = ntohl(p3theader.version);
	p3theader.elements.offset = ntohl(p3theader.elements.offset);
	p3theader.elements.size = ntohl(p3theader.elements.size);
	p3theader.ids.offset = ntohl(p3theader.ids.offset);
	p3theader.ids.size = ntohl(p3theader.ids.size);
	p3theader.strings.offset = ntohl(p3theader.strings.offset);
	p3theader.strings.size = ntohl(p3theader.strings.size);
	p3theader.integers.offset = ntohl(p3theader.integers.offset);
	p3theader.integers.size = ntohl(p3theader.integers.size);
	p3theader.decimals.offset = ntohl(p3theader.decimals.offset);
	p3theader.decimals.size = ntohl(p3theader.decimals.size);
	p3theader.files.offset = ntohl(p3theader.files.offset);
	p3theader.files.size = ntohl(p3theader.files.size);
	 
#ifdef OUTPUT_DEBUG_TEXT
  char magic_string[5];
  *((unsigned long*)magic_string) = p3theader.magic;
  magic_string[4] = 0;
  printf ("magic=%s\n", magic_string);
  printf ("version=%d\n", p3theader.version);
  printf ("element offset=%d size=%d\n", p3theader.elements.offset, p3theader.elements.size);
  printf ("id table offset=%d size=%d\n", p3theader.ids.offset, p3theader.ids.size);
  printf ("string table offset=%d size=%d\n", p3theader.strings.offset, p3theader.strings.size);
  printf ("integer array offset=%d size=%d\n", p3theader.integers.offset, p3theader.integers.size);
  printf ("float array offset=%d size=%d\n", p3theader.decimals.offset, p3theader.decimals.size);
  printf ("file table offset=%d size=%d\n", p3theader.files.offset, p3theader.files.size);
#endif
  
  return P3T_OK;
}

int loadP3TStringTable(FILE* p3t_file_handle, p3tHeader& p3theader, stringMap& strings)
{
#ifdef OUTPUT_DEBUG_TEXT
  printf("\n===== Reading Strings =====\n");
#endif
  
  char* string_table_buffer = new char[p3theader.strings.size];
  if (string_table_buffer == 0)
  {
    fclose(p3t_file_handle);
    return P3T_OUT_OF_MEMORY;
  }
  
  fseek(p3t_file_handle, p3theader.strings.offset, SEEK_SET);
  p3tSizeType no_string_table_bytes_read = fread(string_table_buffer, 1, p3theader.strings.size, p3t_file_handle);
  if (no_string_table_bytes_read != p3theader.strings.size)
  {
    delete[]string_table_buffer;
    fclose(p3t_file_handle);
    return P3T_COULD_NOT_READ_STRING_TABLE;
  }
  
  p3tOffsetType next_string_offset = 0;
  while (next_string_offset < p3theader.strings.size)
  {
    std::string next_string(&string_table_buffer[next_string_offset]);
    strings[next_string_offset] = next_string;
    
#ifdef OUTPUT_DEBUG_TEXT
    printf ("%d %d %s\n", next_string_offset, next_string.size(), next_string.c_str());
#endif
    
    next_string_offset += next_string.size() + 1;
  }
  delete[]string_table_buffer;
  
  return P3T_OK;
}

int loadP3TIdTable(FILE* p3t_file_handle, p3tHeader& p3theader, idMap& ids)
{
#ifdef OUTPUT_DEBUG_TEXT
  printf("\n===== Reading IDS =====\n");  
#endif
  
  char* id_table_buffer = new char[p3theader.ids.size];
  if (id_table_buffer == 0)
  {
    fclose(p3t_file_handle);
    return P3T_OUT_OF_MEMORY;
  }
  
  fseek(p3t_file_handle, p3theader.ids.offset, SEEK_SET);
  p3tSizeType no_id_table_bytes_read = fread(id_table_buffer, 1, p3theader.ids.size, p3t_file_handle);
  if (no_id_table_bytes_read != p3theader.ids.size)
  {
    delete[]id_table_buffer;
    fclose(p3t_file_handle);
    return P3T_COULD_NOT_READ_ID_TABLE;
  }
  
  p3tOffsetType next_id_offset = 0;
  while (next_id_offset < p3theader.ids.size)
  {
    p3tOffsetType offset = next_id_offset;
    
    // skip the elementID
    next_id_offset += 4;
    
    std::string id_text(&id_table_buffer[next_id_offset]);
    
    ids[offset] = id_text;
    
#ifdef OUTPUT_DEBUG_TEXT
    printf ("%d %d %s\n", offset, id_text.size(), id_text.c_str());
#endif
    
    next_id_offset += id_text.size() + 1;
  }
  delete[]id_table_buffer;
  
  return P3T_OK;
}

void extractRippedElements(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements);

// ugly atm (copy-paste of p3tRip) but might be more useful later to extract other p3t theme data

int p3tprevRip(const char* p3t_filename,char* destdir, p3tTheme& p3t_theme)
{
	
	FILE *p3tpathcheck = fopen(destdir,"r");
	if(p3tpathcheck)
	{
		fclose(p3tpathcheck);
	}
	else {mkdir(destdir, 0755);}	
		
	FILE* p3t_file_handle = fopen(p3t_filename, "rb");
	if (p3t_file_handle == 0)
	{
		return P3T_CANNOT_OPEN_FILE;
	}

	int result;

	struct p3tHeader p3theader;
	result = loadP3THeader(p3t_file_handle, p3theader);
	if (result != P3T_OK) return result;

	if (p3theader.magic != 0x50335446) return P3T_CORRUPT_P3T_FILE;
  
	stringMap strings;
	result = loadP3TStringTable(p3t_file_handle, p3theader, strings);
	if (result != P3T_OK)    return result;

	idMap ids;
	result = loadP3TIdTable(p3t_file_handle, p3theader, ids);
	if (result != P3T_OK)
	return result;
  
  // ==================== Rip Elements with their attributes
    
  elementMap elements;
  
  fseek(p3t_file_handle, p3theader.elements.offset, SEEK_SET);
      
  p3tOffsetType next_element_offset = 0;
  while (next_element_offset < p3theader.elements.size)
  {
    p3tElement element;
    p3tOffsetType elementID = next_element_offset;
	size_t read_size = 0;
	size_t write_size = 0;
    read_size = fread(&element.header, 1, sizeof(element.header), p3t_file_handle);
	  
	  element.header.name_offset = ntohl(element.header.name_offset);
	  element.header.no_attributes = ntohl(element.header.no_attributes);
	  element.header.parent_offset = ntohl(element.header.parent_offset);
	  element.header.previous_sibling_offset = ntohl(element.header.previous_sibling_offset);
	  element.header.next_sibling_offset = ntohl(element.header.next_sibling_offset);
	  element.header.first_child_offset = ntohl(element.header.first_child_offset);
	  element.header.last_child_offset = ntohl(element.header.last_child_offset);
	   
    element.offset = elementID;

    
    next_element_offset += sizeof(element.header);
    
    unsigned int attribute_index;
    for (attribute_index = 0; attribute_index < element.header.no_attributes; ++attribute_index)
    {
      struct p3tAttribute attribute;
      read_size = fread(&attribute.header, 1, sizeof(attribute.header), p3t_file_handle);
		
		attribute.header.name_offset = ntohl(attribute.header.name_offset);
		attribute.header.type = ntohl(attribute.header.type);
		attribute.header.value.location.offset = ntohl(attribute.header.value.location.offset);
		attribute.header.value.location.size = ntohl(attribute.header.value.location.size);
		 

      next_element_offset += sizeof(attribute.header);
      
      switch (attribute.header.type)
      {
        case 1: 
          break;
          
        case 2: 
          break;
          
        case 3: 
          break;
          
        case 6:
        {
          p3tOffsetType current_file_position = ftell(p3t_file_handle);
          attribute.file_data = new unsigned char[attribute.header.value.location.size];
          fseek(p3t_file_handle, p3theader.files.offset + attribute.header.value.location.offset, SEEK_SET);
          read_size = fread(attribute.file_data, 1, attribute.header.value.location.size, p3t_file_handle);
          fseek(p3t_file_handle, current_file_position, SEEK_SET);

		  if(strcasecmp(strings[element.header.name_offset].c_str(),"info") == 0)
          {
			char gim_filename[100];
			sprintf(gim_filename, "%s", strings[element.header.name_offset].c_str());
			strcat (gim_filename,"_");
			strcat (gim_filename,strings[attribute.header.name_offset].c_str());
			strcat (gim_filename,".gim");
            			  
            attribute.file_type = P3T_GIM_FILE;
            bool blib=false; // required for gim2png fixes
            void* uncompressed_buffer;
            int uncompressed_size = uncompressGIMBuffer(attribute.file_data, attribute.header.value.location.size, &uncompressed_buffer);
            
            delete[](char*)attribute.file_data;
            attribute.file_data = 0;
            
            if (uncompressed_size < 0)
            {
              attribute.file_size = 0;
              delete[](char*)uncompressed_buffer;
            }
            else
            {              
              attribute.file_data = new unsigned char[uncompressed_size];
              
              if (attribute.file_data == 0)
              {
                attribute.file_size = 0;
                

                printf("*** COULD NOT ALLOCATE MEMORY FOR UNCOMPRESSED FILE\n");

              }
              else
              {
                memcpy(attribute.file_data, uncompressed_buffer, uncompressed_size);
                attribute.file_size = uncompressed_size;
                blib=true; //true if gim was uncompressed
              }
              
              delete[](char*)uncompressed_buffer;
            }

				if (extractgim)
				{
					char fullpath[150];
					strcpy(fullpath,destdir);
					strcat(fullpath,"/");					
					strcat(fullpath,gim_filename);
					
					FILE* gim_file_handle = fopen((fullpath), "wb");
					// if blib is false take original filesize else the uncompressed one
					if (!blib) write_size = fwrite(attribute.file_data, 1, attribute.header.value.location.size, gim_file_handle);
					else write_size = fwrite(attribute.file_data, 1, uncompressed_size, gim_file_handle);
					fclose(gim_file_handle);
					char *suf;
					char ogim_filename[150];
					strcpy(ogim_filename,fullpath);
					suf = strstr(fullpath, ".gim");
					strcpy(suf, ".png");				    
					gim2png( ogim_filename, fullpath);

				}
          }
          
          break;
          }
          case 7: 
          break;
          
          default: 
          ;
      }      
      element.attributes.push_back(attribute);
    }
    
    elements[elementID] = element;
  }
  
  fclose(p3t_file_handle);
    
  extractRippedElements(p3t_theme, strings, ids, elements);

  return P3T_OK;
}

int p3tRip(const char* p3t_filename,char* destdir, p3tTheme& p3t_theme)
{
	
	FILE *p3tpathcheck = fopen(destdir,"r");
	if(p3tpathcheck)
	{
		fclose(p3tpathcheck);
	}
	else 
	{mkdir(destdir, 0755);}	

	FILE* p3t_file_handle = fopen(p3t_filename, "rb");
	if (p3t_file_handle == 0)
	{
		return P3T_CANNOT_OPEN_FILE;
	}

	int result;

	struct p3tHeader p3theader;
	result = loadP3THeader(p3t_file_handle, p3theader);
	if (result != P3T_OK) return result;

	if (p3theader.magic != 0x50335446) return P3T_CORRUPT_P3T_FILE;
  
	stringMap strings;
	result = loadP3TStringTable(p3t_file_handle, p3theader, strings);
	if (result != P3T_OK)    return result;

	idMap ids;
	result = loadP3TIdTable(p3t_file_handle, p3theader, ids);
	if (result != P3T_OK)
	return result;
  
  // ==================== Rip Elements with their attributes
    
  elementMap elements;
  
  fseek(p3t_file_handle, p3theader.elements.offset, SEEK_SET);
      
  p3tOffsetType next_element_offset = 0;
  while (next_element_offset < p3theader.elements.size)
  {
    p3tElement element;
    p3tOffsetType elementID = next_element_offset;
    size_t read_size = 0; 
    read_size = fread(&element.header, 1, sizeof(element.header), p3t_file_handle);
	  
	  element.header.name_offset = ntohl(element.header.name_offset);
	  element.header.no_attributes = ntohl(element.header.no_attributes);
	  element.header.parent_offset = ntohl(element.header.parent_offset);
	  element.header.previous_sibling_offset = ntohl(element.header.previous_sibling_offset);
	  element.header.next_sibling_offset = ntohl(element.header.next_sibling_offset);
	  element.header.first_child_offset = ntohl(element.header.first_child_offset);
	  element.header.last_child_offset = ntohl(element.header.last_child_offset);
	   
    element.offset = elementID;
#ifdef OUTPUT_DEBUG_TEXT
    printf("\n===== Element %d =====\n name=%s no_atrr=%d prev_sib=%d next_sib=%d frst_cld=%d last_cld=%d ", elementID, 
           strings[element.header.name_offset].c_str(), 
           element.header.no_attributes,
           element.header.previous_sibling_offset, element.header.next_sibling_offset,
           element.header.first_child_offset, element.header.last_child_offset);
    if (element.header.parent_offset != (p3tOffsetType)-1)
    {
      printf("parent=%s\n", strings[elements[element.header.parent_offset].header.name_offset].c_str());
    }
    else
      printf("ROOT\n");
#endif
    
    next_element_offset += sizeof(element.header);
    
    unsigned int attribute_index;
    for (attribute_index = 0; attribute_index < element.header.no_attributes; ++attribute_index)
    {
      struct p3tAttribute attribute;
      read_size = fread(&attribute.header, 1, sizeof(attribute.header), p3t_file_handle);
		
		attribute.header.name_offset = ntohl(attribute.header.name_offset);
		attribute.header.type = ntohl(attribute.header.type);
		attribute.header.value.location.offset = ntohl(attribute.header.value.location.offset);
		attribute.header.value.location.size = ntohl(attribute.header.value.location.size);
		 
      
#ifdef OUTPUT_DEBUG_TEXT
      printf("%d AttributeName=%s type=%d value=", next_element_offset, strings[attribute.header.name_offset].c_str(), attribute.header.type);
#endif
      next_element_offset += sizeof(attribute.header);
      
      switch (attribute.header.type)
      {
        case 1: 
#ifdef OUTPUT_DEBUG_TEXT
          printf("%d\n", attribute.header.value.integer);
#endif
          break;
          
        case 2: 
#ifdef OUTPUT_DEBUG_TEXT
          printf("%f\n", attribute.header.value.decimal); 
#endif
          
          break;
          
        case 3: 
#ifdef OUTPUT_DEBUG_TEXT
          printf("%s [%d]\n", strings[attribute.header.value.location.offset].c_str(), attribute.header.value.location.size); 
#endif
          
          break;
          
        case 6:
        {
#ifdef OUTPUT_DEBUG_TEXT
          printf("%d %d\n", attribute.header.value.location.offset, attribute.header.value.location.size);
#endif
          
          p3tOffsetType current_file_position = ftell(p3t_file_handle);
          attribute.file_data = new unsigned char[attribute.header.value.location.size];
          fseek(p3t_file_handle, p3theader.files.offset + attribute.header.value.location.offset, SEEK_SET);
          read_size = fread(attribute.file_data, 1, attribute.header.value.location.size, p3t_file_handle);
          fseek(p3t_file_handle, current_file_position, SEEK_SET);

          if (strings[element.header.name_offset] == "se")
          {
			char vag_filename[100];
			sprintf(vag_filename, "%s", strings[element.header.name_offset].c_str());
//			strcat (vag_filename,"_");
//			strcat (vag_filename,strings[attribute.header.name_offset].c_str());
			char vcountbuf[10]; 

			vagcount+=1;
			sprintf(vcountbuf, "%i", vagcount);
         
			strcat (vag_filename,"_");
			strcat (vag_filename,vcountbuf);
			strcat (vag_filename,".vag");
				
            
            attribute.file_type = P3T_VAG_FILE;
            attribute.file_size = attribute.header.value.location.size;
            
#ifdef OUTPUT_DEBUG_TEXT
            printf("    VAG sound\n");
#endif

				if(extractvag)
				{
					size_t write_size = 0;
					char fullpath[150];
					strcpy(fullpath,destdir);
					strcat(fullpath,"/");					
					strcat(fullpath,vag_filename);
								
					FILE* vag_file_handle = fopen((fullpath), "wb");
					write_size = fwrite(attribute.file_data, 1, attribute.header.value.location.size, vag_file_handle);
					fclose(vag_file_handle);  
				
					char *suf;
					char ovag_filename[150];
					strcpy(ovag_filename,fullpath);
					suf = strstr(fullpath, ".vag");
					strcpy(suf, ".wav");				    
					vag2wav( ovag_filename, fullpath);
				}

			}
			else if (strings[element.header.name_offset] == "bgimage")
			{
			char jpg_filename[100];
			
			sprintf(jpg_filename, "%s", strings[attribute.header.name_offset].c_str());
			char countbuf[10]; 

			if (strcasecmp(jpg_filename,"hd") == 0 && extracthd)
			{
				hdcount+=1;
				sprintf(countbuf, "%i", hdcount);
            }
            
  			if (strcasecmp(jpg_filename,"sd") == 0 && extractsd)
			{
				sdcount+=1;
				sprintf(countbuf, "%i", sdcount);
            }
         
			if
			(
				(strcasecmp(jpg_filename,"sd") == 0 && extractsd) ||
				(strcasecmp(jpg_filename,"hd") == 0 && extracthd)
			)
			{
				strcat (jpg_filename,"_");
				strcat (jpg_filename,countbuf);
				strcat (jpg_filename,".jpg");

				attribute.file_type = P3T_JPG_FILE;
				attribute.file_size = attribute.header.value.location.size;

				char fullpath[150];
				strcpy(fullpath,destdir);
				strcat(fullpath,"/backgrounds/");
				
				FILE *p3tbgpathcheck = fopen(fullpath,"r");
				if(p3tbgpathcheck)
				{
					fclose(p3tbgpathcheck);
				}
				else 
				{mkdir(fullpath, 0755);}				
							
				strcat(fullpath,jpg_filename);
				size_t write_size = 0;

				FILE* jpg_file_handle = fopen((fullpath), "wb");
				write_size = fwrite(attribute.file_data, 1, attribute.header.value.location.size, jpg_file_handle);
				fclose(jpg_file_handle);
			}
          }
          else
          {
			char gim_filename[100];
			sprintf(gim_filename, "%s", strings[element.header.name_offset].c_str());
			strcat (gim_filename,"_");
			strcat (gim_filename,strings[attribute.header.name_offset].c_str());
			char gimcountbuf[10]; 

         

			if(strcasecmp(gim_filename,"icon_src") == 0)
			{
				gimcount+=1;
				sprintf(gimcountbuf, "%i", gimcount);
				strcat (gim_filename,"_");
				strcat (gim_filename,gimcountbuf);
			}
			strcat (gim_filename,".gim");


            			  
            attribute.file_type = P3T_GIM_FILE;
            bool blib=false; // required for gim2png fixes
            void* uncompressed_buffer;
            int uncompressed_size = uncompressGIMBuffer(attribute.file_data, attribute.header.value.location.size, &uncompressed_buffer);
            
            delete[](char*)attribute.file_data;
            attribute.file_data = 0;
            
            if (uncompressed_size < 0)
            {
              attribute.file_size = 0;
				delete[](char*)uncompressed_buffer;
#ifdef OUTPUT_ERROR_TEXT
              printf("*** Element=%s AttributeName=%s Type=%d\n", 
                     strings[element.header.name_offset].c_str(), 
                     strings[attribute.header.name_offset].c_str(), attribute.header.type); 
#ifndef OUTPUT_DEBUG_TEXT
              printf("\n");
#else
              printf("*** COULD NOT UNCOMPRESS\n");
#endif
              
#endif
            }
            else
            {              
              attribute.file_data = new unsigned char[uncompressed_size];
              
              if (attribute.file_data == 0)
              {
                attribute.file_size = 0;
                

                printf("*** COULD NOT ALLOCATE MEMORY FOR UNCOMPRESSED FILE\n");

              }
              else
              {
                memcpy(attribute.file_data, uncompressed_buffer, uncompressed_size);
                attribute.file_size = uncompressed_size;
                blib=true; //true if gim was uncompressed
              }
              
#ifdef OUTPUT_DEBUG_TEXT
              printf("    uncommpressedsize=%d\n", uncompressed_size);
#endif
              
              delete[](char*)uncompressed_buffer;
            }

				if (extractgim)
				{
					char fullpath[150];
					strcpy(fullpath,destdir);
					strcat(fullpath,"/");					
					strcat(fullpath,gim_filename);
					
					FILE* gim_file_handle = fopen((fullpath), "wb");
					size_t write_size = 0;
					// if blib is false take original filesize else the uncompressed one
					if (!blib) write_size = fwrite(attribute.file_data, 1, attribute.header.value.location.size, gim_file_handle);
					else write_size = fwrite(attribute.file_data, 1, uncompressed_size, gim_file_handle);
					fclose(gim_file_handle);
					char *suf;
					char ogim_filename[150];
					strcpy(ogim_filename,fullpath);
					suf = strstr(fullpath, ".gim");
					strcpy(suf, ".png");				    
					gim2png( ogim_filename, fullpath);

				}
          }
          
          break;
          }
          case 7: 
#ifdef OUTPUT_DEBUG_TEXT
          printf("%s[%d]\n", ids[attribute.header.value.location.offset].c_str(), attribute.header.value.location.size); 
#endif
          
          break;
          
          default: 
#ifdef OUTPUT_DEBUG_TEXT
          printf("N/A\n");
#endif
          
          ;
      }      
      element.attributes.push_back(attribute);
    }
    
    elements[elementID] = element;
  }
  
  fclose(p3t_file_handle);
    
  extractRippedElements(p3t_theme, strings, ids, elements);
	hdcount=0;
	sdcount=0;
	gimcount=0;
	vagcount=0; 
  return P3T_OK;
}

void extractInfoElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);
void extractLocalizedInfoElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);
void extractSoundEffectElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);
void extractIconElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);
void extractPointerElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);
void extractNotificationElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);
void extractBGImageElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);
void extractFontElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);
void extractColorElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element);

#define GET_ATTRIBUTE_INT_VALUE attribute.header.value.integer
#define GET_ATTRIBUTE_STRING_VALUE strings[attribute.header.value.location.offset]
#define GET_ATTRIBUTE_FILE_DATA attribute.file_data
#define GET_ATTRIBUTE_FILE_SIZE attribute.file_size
#define GET_ATTRIBUTE_ID_VALUE ids[attribute.header.value.location.offset]

void extractRippedElements(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements)
{  
  p3t_theme.bgimagetable.bgimage.clear();
  p3t_theme.icontable.icon.clear();
  p3t_theme.pointertable.pointer.clear();
  
  // Extract Elements and populate p3tTheme structure
  for (elementMap::const_iterator element_iterator = elements.begin(); element_iterator != elements.end(); ++element_iterator)
  {
    const p3tElement& element = (const p3tElement&)element_iterator->second;
    
    if (strings[element.header.name_offset] == "info")
    {
      extractInfoElement(p3t_theme, strings, ids, elements, element);
    }
    else if (strings[element.header.name_offset] == "localizedinfo")
    {
      extractLocalizedInfoElement(p3t_theme, strings, ids, elements, element);
    }
    else if (strings[element.header.name_offset] == "se")
    {
      extractSoundEffectElement(p3t_theme, strings, ids, elements, element);
    }
    else if (strings[element.header.name_offset] == "icon")
    {
      extractIconElement(p3t_theme, strings, ids, elements, element);
    }
    else if (strings[element.header.name_offset] == "pointer")
    {
      extractPointerElement(p3t_theme, strings, ids, elements, element);
    }
    else if (strings[element.header.name_offset] == "notification")
    {
      extractNotificationElement(p3t_theme, strings, ids, elements, element);
    }
    else if (strings[element.header.name_offset] == "bgimage")
    {
      extractBGImageElement(p3t_theme, strings, ids, elements, element);
    }
    else if (strings[element.header.name_offset] == "font")
    {
      extractFontElement(p3t_theme, strings, ids, elements, element);
    }
    else if (strings[element.header.name_offset] == "color")
    {
      extractColorElement(p3t_theme, strings, ids, elements, element);
    }
  }
}

void extractInfoElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "comment")
    {
      p3t_theme.infotable.info.comment = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "previewsize")
    {
      p3t_theme.infotable.info.previewsize = GET_ATTRIBUTE_INT_VALUE;
    }
    else if (attribute_name == "name")
    {
      p3t_theme.infotable.info.name = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "author")
    {
      p3t_theme.infotable.info.author = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "url")
    {
      p3t_theme.infotable.info.url = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "authoriconsize")
    {
      p3t_theme.infotable.info.authoriconsize = GET_ATTRIBUTE_INT_VALUE;
    }
    else if (attribute_name == "authoricon")
    {
      p3t_theme.infotable.info.authoricon.file_data = GET_ATTRIBUTE_FILE_DATA;
      p3t_theme.infotable.info.authoricon.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else if (attribute_name == "iconsize")
    {
      p3t_theme.infotable.info.iconsize = GET_ATTRIBUTE_INT_VALUE;
    }
    else if (attribute_name == "version")
    {
      p3t_theme.infotable.info.version = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "mtime")
    {
      p3t_theme.infotable.info.mtime = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "genre")
    {
      p3t_theme.infotable.info.genre = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "preview")
    {
      p3t_theme.infotable.info.preview.file_data = GET_ATTRIBUTE_FILE_DATA;
      p3t_theme.infotable.info.preview.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else if (attribute_name == "icon")
    {
      p3t_theme.infotable.info.icon.file_data = GET_ATTRIBUTE_FILE_DATA;
      p3t_theme.infotable.info.icon.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown InfoElement attribute: %s ***\n", attribute_name.c_str());
#endif
    }
  }
}

void extractLocalizedInfoElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  p3tLocalizedInfoElement local_info;
  
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "locale")
    {
      local_info.locale = GET_ATTRIBUTE_ID_VALUE;
    }
    else if (attribute_name == "comment")
    {
      local_info.comment = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "name")
    {
      local_info.name = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "author")
    {
      local_info.author = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "url")
    {
      local_info.url = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "version")
    {
      local_info.version = GET_ATTRIBUTE_STRING_VALUE;
    }
    else if (attribute_name == "genre")
    {
      local_info.genre = GET_ATTRIBUTE_STRING_VALUE;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown LocalizedInfoElement attribute: %s\n", attribute_name.c_str());
#endif
    }
  }
  
  p3t_theme.infotable.localizedinfo.push_back(local_info);
}

void extractSoundEffectElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  p3tSoundEffectElement se;
  se.left.file_data = 0;
  se.left.file_size = 0;
  se.right.file_data = 0;
  se.right.file_size = 0;
  
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "src")
    {
      se.left.file_data = GET_ATTRIBUTE_FILE_DATA;
      se.left.file_size = GET_ATTRIBUTE_FILE_SIZE;
      se.right.file_data = 0;
      se.right.file_size = 0;
    }
    else if (attribute_name == "left")
    {
      se.left.file_data = GET_ATTRIBUTE_FILE_DATA;
      se.left.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else if (attribute_name == "right")
    {
      se.right.file_data = GET_ATTRIBUTE_FILE_DATA;
      se.right.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else if (attribute_name == "id")
    {
      se.id = GET_ATTRIBUTE_ID_VALUE;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown SEElement attribute: %s\n", attribute_name.c_str());
#endif
    }
  }
  
  p3t_theme.setable.se.push_back(se);
}

void extractIconElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  p3tIconElement icon;
  icon.src.file_data = 0;
  icon.src.file_size = 0;
  icon.size = 0;
  
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "src")
    {
      icon.src.file_data = GET_ATTRIBUTE_FILE_DATA;
      icon.src.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else if (attribute_name == "id")
    {
      icon.id = GET_ATTRIBUTE_ID_VALUE;
    }
    else if (attribute_name == "size")
    {
      icon.size = GET_ATTRIBUTE_INT_VALUE;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown IconElement attribute: %s\n", attribute_name.c_str());
#endif
    }
  }
    
  p3t_theme.icontable.icon.push_back(icon);
}

void extractPointerElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  p3tPointerElement pointer;
  pointer.src.file_data = 0;
  pointer.src.file_size = 0;
  pointer.size = 0;
  
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "base_x")
    {
      pointer.base_x = GET_ATTRIBUTE_INT_VALUE;
    }
    else  if (attribute_name == "src")
    {
      pointer.src.file_data = GET_ATTRIBUTE_FILE_DATA;
      pointer.src.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else if (attribute_name == "size")
    {
      pointer.size = GET_ATTRIBUTE_INT_VALUE;
    }
    else if (attribute_name == "id")
    {
      pointer.id = GET_ATTRIBUTE_ID_VALUE;
    }
    else if (attribute_name == "base_y")
    {
      pointer.base_y = GET_ATTRIBUTE_INT_VALUE;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown PointerElement attribute: %s\n", attribute_name.c_str());
#endif
    }
  }
  
  p3t_theme.pointertable.pointer.push_back(pointer);
}

void extractNotificationElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "src")
    {
      p3t_theme.notification.src.file_data = GET_ATTRIBUTE_FILE_DATA;
      p3t_theme.notification.src.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else if (attribute_name == "size")
    {
      p3t_theme.notification.size = GET_ATTRIBUTE_INT_VALUE;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown NotificationElement attribute: %s\n", attribute_name.c_str());
#endif
    }    
  }
}

void extractBGImageElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  p3tBGImageElement bgimage;
  
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "hd")
    {
      bgimage.hd.file_data = GET_ATTRIBUTE_FILE_DATA;
      bgimage.hd.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else if (attribute_name == "sd")
    {
      bgimage.sd.file_data = GET_ATTRIBUTE_FILE_DATA;
      bgimage.sd.file_size = GET_ATTRIBUTE_FILE_SIZE;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown BGImageElement attribute: %s\n", attribute_name.c_str());
#endif
    }
  }
  
  p3t_theme.bgimagetable.bgimage.push_back(bgimage);
}

void extractFontElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "selection")
    {
      p3t_theme.font.selection = GET_ATTRIBUTE_INT_VALUE;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown FontElement attribute: %s\n", attribute_name.c_str());
#endif
    }
  }
}

void extractColorElement(p3tTheme& p3t_theme, stringMap& strings, idMap& ids, elementMap& elements, const p3tElement& element)
{
  for (p3tAttributeList::const_iterator attribute_iterator = element.attributes.begin(); attribute_iterator != element.attributes.end(); ++attribute_iterator)
  {
    const p3tAttribute& attribute = (const p3tAttribute&)*attribute_iterator; 
    std::string attribute_name = strings[attribute.header.name_offset];
    
    if (attribute_name == "selection")
    {
      p3t_theme.color.selection = GET_ATTRIBUTE_INT_VALUE;
      p3tcolor=p3t_theme.color.selection;
    }
    else
    {
#ifdef OUTPUT_ERROR_TEXT
      printf("*** Unknown ColorElement attribute: %s\n", attribute_name.c_str());
#endif
    }
  }
}


