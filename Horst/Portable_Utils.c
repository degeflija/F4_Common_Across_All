/**
  * ############################################################################
  * @file     Portable_Utils.c
  * @brief    New Vario
  * @author   Horst Rupp
  * @brief    This library contains all portable basic functions.
  * ############################################################################
  */

//
//  Includes
//
#include  "Portable_Utils.h"

// *****************************************************************************
//  Invert bits in a byte
// *****************************************************************************
//
uint8_t InvertBitsInAByte ( uint8_t pattern )
{
  uint8_t loc_pattern = 0;
  for ( uint8_t i = 0; i < 8; i++ )
    if ( ( pattern & ( 1 << i ) ) != 0 )
      loc_pattern |= 1 << ( 7 - i );
  return loc_pattern;
}

// *****************************************************************************
//  Returns the no of the first bit set - counting from low to high bits
// *****************************************************************************
//
uint32_t  FirstBitNoSet   ( uint32_t pattern, uint8_t depth )
{
  uint32_t  i     = 0,
            j     = 0;
  uint8_t   found = 0;
  uint32_t  l_pattern;
  do
  {
    l_pattern = 1 << i;
    if ( ( pattern & l_pattern ) != 0 )
    {
      found = 1;
      j = i;
    }
  }
  while ( ( ! found ) && ( i++ < depth ) );
    return j;

}

// *****************************************************************************
//  Returns 1 if x is in range [low..high], else 0
// *****************************************************************************
//
uint8_t  inRange ( int32_t  low, int32_t high, int32_t x)
{
  uint8_t   y = ((x-low) <= (high-low));
    return  y;
}

// *****************************************************************************
// count bits in 8 bit word
// *****************************************************************************
//
uint8_t Count_8_Bits ( uint8_t bit_pattern )
{
  uint8_t count = 0;
  for ( uint8_t i = 0; i < 8; i++ )
  {
    if ( ( bit_pattern & 1 ) == 1 )
      count++;
    bit_pattern = bit_pattern >> 1;
  }
  return count;
}

// *****************************************************************************
// count bits in 16 bit word
// *****************************************************************************
//
uint8_t Count_16_Bits ( uint16_t bit_pattern )
{
  uint8_t count = 0;
  for ( uint8_t i = 0; i < 16; i++ )
  {
    if ( ( bit_pattern & 1 ) == 1 )
      count++;
    bit_pattern = bit_pattern >> 1;
  }
  return count;
}

// *****************************************************************************
// portable_isspace
// *****************************************************************************
//
uint8_t  portable_isspace  ( char x )
{
  if ( x == ' ' || x == 9 )
    return 1;
  else
    return 0;
}

// *****************************************************************************
// portable_isdigit
// *****************************************************************************
//
uint8_t  portable_isdigit ( char x )
{
  if ( x >= '0' && x <= '9' )
    return 1;
  else
    return 0;
}


// *****************************************************************************
// portable_islowcasechar
// *****************************************************************************
//
uint8_t  portable_islowcasechar ( char x )
{
  if ( x >= 'a' && x <= 'z' )
    return 1;
  else
    return 0;
}


// *****************************************************************************
// portable_isuppcasechar
// *****************************************************************************
//
uint8_t  portable_isuppcasechar ( char x )
{
  if ( x >= 'A' && x <= 'Z' )
    return 1;
  else
    return 0;
}


// *****************************************************************************
// portable_isprintablechar
// *****************************************************************************
//
uint8_t  portable_isprintablechar ( char x )
{
  if ( x >= 32 && x <= 126 )
    return 1;
  else
    return 0;
}


// *****************************************************************************
// portable_skipleadchar
// *****************************************************************************
//
void portable_skipleadchar ( char * dest, uint8_t drop_count )
{
  uint16_t i;

  while ( ( drop_count-- > 0 ) && ( dest[0] != 0 ) )
  {
    for ( i = 0; i < portable_strlen ( dest ); i++ )
    {
      dest[i] = dest[i+1];
    }
    //  NOTE: the loop above already copies the terminating NUL in its
    //  last iteration ( dest[i] = dest[i+1] where dest[i+1] was the old
    //  NUL ), so the string is correctly terminated at this point.
    //  A trailing "dest[i+1] = 0;" here would write one byte PAST that
    //  NUL - a 1-byte out-of-bounds write - and has been removed.
  }
}


// ****************************************************************************
uint16_t portable_strlen ( const char * ValText )
// ****************************************************************************
{
  uint16_t  i;

  i = 0;
  while ( ValText[i++] != 0 )
  {
    ;
  }
  return --i;

} // portable_strlen

// *****************************************************************************
// strcpy replacement
// *****************************************************************************
//
char * portable_strcpy ( char * dest, const char * source )
{
  uint16_t   l_size = portable_strlen ( source );
  uint16_t   i;
  for ( i = 0; i < l_size; i++ )
  {
    dest[i] = source[i];
  }
  dest[i] = 0;
  return dest;
}

// *****************************************************************************
// strcpy replacement
// *****************************************************************************
//
char * portable_strcpy_safe ( char * dest, const char * source, uint8_t dest_size )
{
  uint16_t  i;
  uint16_t    l_size = portable_strlen ( source );
  if ( l_size > dest_size ) {
    l_size = dest_size;
  }

  for ( i = 0; i < l_size; i++ )
  {
    dest[i] = source[i];
  }

  dest[i] = 0;
  return dest;
}

// *****************************************************************************
// portable_strcat replacement
// *****************************************************************************
//
char * portable_strcat ( char * dest, const char * source )
{

  uint16_t  j;
  uint16_t  d = portable_strlen(dest);
  uint16_t  s = portable_strlen(source);

  for ( j = 0; j < s; j++ )
  {
    dest[d+j] = source[j];
  }
  dest[d+j] = 0;
  return dest;
}

// *****************************************************************************
// portable_strcat_bounded : same as portable_strcat, but NEVER writes past
// ( dest_capacity - 1 ), always leaves room for the terminating NUL.
// Truncates the appended content rather than overflowing dest.
// Use this instead of portable_strcat() whenever dest is a fixed-size
// buffer being built up from several concatenations of caller-supplied /
// variable-length content ( e.g. log line assembly ).
// *****************************************************************************
//
char * portable_strcat_bounded ( char * dest, const char * source, uint16_t dest_capacity )
{
  uint16_t  j;
  uint16_t  d = portable_strlen(dest);
  uint16_t  s;
  uint16_t  l_room;

  if ( ( dest_capacity == 0 ) || ( d >= ( dest_capacity - 1 ) ) )
  {
    //  already full ( or zero-capacity buffer ) - just make sure
    //  whatever fits is properly NUL-terminated
    //
    if ( dest_capacity > 0 )
      dest[dest_capacity - 1] = 0;
    return dest;
  }

  l_room = ( dest_capacity - 1 ) - d;    // bytes left, minus room for NUL
  s      = portable_strlen(source);
  if ( s > l_room )
    s = l_room;                          // truncate - never overflow

  for ( j = 0; j < s; j++ )
  {
    dest[d+j] = source[j];
  }
  dest[d+j] = 0;
  return dest;
}


// *****************************************************************************
// portable_strcut replacement
// *****************************************************************************
//
char * portable_strcut ( char * dest, const uint16_t shorten_by )
{

  uint16_t  j;
  uint16_t  b = portable_strlen(dest);

  for ( j = 1; j <= shorten_by; j++ )
  {
    dest[b-j] = 0;
  }
  return dest;
}


// *****************************************************************************
// portable_strcmp
// *****************************************************************************
//
#define  StringOneIsLess    -1
#define  StringOneIsGreater  1
#define  StringsAreEqual     0

int8_t  portable_strcmp ( const char *StringOne, const char *StringTwo )
{
  uint16_t i;

  // Evaluates if both strings have the same length.
  if  ( portable_strlen ( StringOne ) != portable_strlen ( StringTwo ) ) {
    // Given that the strings have an unequal length, it compares between both
    // lengths.
    if  ( portable_strlen ( StringOne ) < portable_strlen ( StringTwo ) ) {
      return ( StringOneIsLess );
    }
    if  ( portable_strlen ( StringOne ) > portable_strlen ( StringTwo ) ) {
      return ( StringOneIsGreater );
    }
  }


  // Since both strings are equal in length...
  for ( i = 0; i < portable_strlen ( StringOne ); i++ ) {
    // It goes comparing letter per letter.
    if  ( StringOne [ i ] != StringTwo [ i ] ) {
      if  ( StringOne [ i ] < StringTwo [ i ] ) {
        return ( -i-1 );
      }
      if  ( StringOne [ i ] > StringTwo [ i ] ) {
        return ( i+1 );
      }
    }
  }
  // If it ever reaches this part, it means they are equal.
  return ( StringsAreEqual );
}

// *****************************************************************************
// portable_strtoupper replacement
// *****************************************************************************
//
char * portable_strtoupper ( char * thestring )
{
   uint16_t i;
   for (i = 0; thestring[i]!='\0'; i++) {
      if(thestring[i] >= 'a' && thestring[i] <= 'z')
      {
        thestring[i] = thestring[i] - 32;
      }
   }
   return thestring;
}

// *****************************************************************************
// portable_strtolower replacement
// *****************************************************************************
//
char * portable_strtolower ( char * thestring )
{
   int i;
   for (i = 0; thestring[i]!='\0'; i++)
   {
      if(thestring[i] >= 'A' && thestring[i] <= 'Z')
      {
        thestring[i] = thestring[i] + 32;
      }
   }
   return thestring;
}

// *****************************************************************************
// trim trailing character
// *****************************************************************************
//
char * portable_trimtrailing (char *input, char chartotrim )
{
  //  BUGFIX: was comparing input[strlen(input)], which is always the NUL
  //  terminator itself - never equal to chartotrim, so this never trimmed
  //  anything. Correct index is strlen(input) - 1, guarded against the
  //  empty-string case to avoid underflowing an unsigned index.
  //
  uint16_t  x = portable_strlen(input);
  while ( ( x > 0 ) && ( input[x-1] == chartotrim ) )
  {
    input[x-1] = 0;
    x--;
  }
  return input;
}

// *****************************************************************************
// trim leading character
// *****************************************************************************
//
char * portable_trimleading ( char *input, char chartotrim )
{
  while ( input[0] == chartotrim )
  {
    portable_skipleadchar ( input, 1 );
  }
  return input;
}

// *****************************************************************************
// trimanywhere
// *****************************************************************************
//
char * portable_trimanywhere ( char *input, char chartotrim )
{
  int32_t   i;
  uint16_t  j;

  for (i = 0; input[i] != '\0'; i++) {
    if ( input[i] == chartotrim )
    {
      for ( j = i; j < portable_strlen(input); j++ )
      {
        input[j] = input[j+1];
      }
      input[j+1] = 0;
    }
  }
  return input;
}

// *****************************************************************************
// portable_compress
// *****************************************************************************
//
char * portable_compress ( char *input )
{
  int16_t   i, j, k;

  i = 0;
  while ( input[i] != '\0')
  {
    if ( ( input[i] == ' ' ) || ( input[i] == 9 ) )
    {
      k = portable_strlen(input);
      for ( j = i; j < k ; j++ )
      {
        input[j] = input[j+1];
      }
      input[j+1] = 0;
      i--;
    }
    i++;
  }
  return input;
}

// ****************************************************************************
// Reverses a string 'str' of length 'len'
// ****************************************************************************
//
void portable_strreverse(char* str, int len)
{
  int i = 0, j = len - 1, temp;
  while (i < j)
  {
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
  i = 1;
}

// ****************************************************************************
// bumps charS <x> into a char array <res>
//  until portable_strlen(res) == res_len is reached
// ****************************************************************************
//
void Counted_PreFix_Char ( char x, char* res, uint8_t max_len )
{
  uint8_t  i, l;

  l = portable_strlen ( res );

  for ( i = max_len - l; i > 0 ; i-- )
  {
    PreFix_Char ( x,  res );
  }
}

// ****************************************************************************
// bumps char <x> into a char array <res>
// assumption <res> contains a zero terminated char string
// CAUTION : No size checking !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ****************************************************************************
//
void PreFix_Char ( char x, char* res )
{
  uint8_t  i, j;
  j = portable_strlen(res);
  for ( i = j; i >= 1 ; i-- )
  {
    res[i] = res[i-1];
  }
  res[j+1] = 0;
  res[i] = x;
}

// ****************************************************************************
// same as PreFix_Char, but never writes past ( max_len - 1 ), always leaves
// room for the terminating NUL. Silently drops the char if already full.
// ****************************************************************************
//
void PreFix_Char_Bounded ( char x, char* res, uint16_t max_len )
{
  uint16_t  i, j;
  j = portable_strlen(res);
  if ( j >= ( max_len - 1 ) )
    return;                    //  no room left - drop the char, stay safe

  for ( i = j; i >= 1 ; i-- )
  {
    res[i] = res[i-1];
  }
  res[j+1] = 0;
  res[i] = x;
}

// ****************************************************************************
// concats so many char <x> at the end of char array <res>
// that portable_strlen is == res_len
//  does nothing if res_len already reached
// assumption <res> contains a zero terminated char string
// ****************************************************************************
//
void Counted_PostFix_Char ( char x, char* res, uint8_t max_len )
{
  uint8_t  ll;
  ll = portable_strlen ( res );

  while ( ll < max_len )
  {
    PostFix_Char ( x, res  );
    ll = portable_strlen ( res );
  }
}

// ****************************************************************************
// concats char <x> at the end of char array <res>
// assumption <res> contains a zero terminated char string
// CAUTION : No size checking !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ****************************************************************************
//
void PostFix_Char ( char x, char* res )
{
  uint8_t  ll;

  ll = portable_strlen ( res );
  res[ll] = x;
  res[ll+1] = 0;
}

// ****************************************************************************
// same as PostFix_Char, but never writes past ( max_len - 1 ), always leaves
// room for the terminating NUL. Silently drops the char if already full.
// ****************************************************************************
//
void PostFix_Char_Bounded ( char x, char* res, uint16_t max_len )
{
  uint8_t  ll;

  ll = portable_strlen ( res );
  if ( ll >= ( max_len - 1 ) )
    return;                    //  no room left - drop the char, stay safe

  res[ll]   = x;
  res[ll+1] = 0;
}

// ****************************************************************************
char* portable_charreplace (char* str, char find, char replace)
// ****************************************************************************
//
{
    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}

// ****************************************************************************
// Search and replace a string with another string , in a string
// ****************************************************************************
char * portable_strreplace(char *search , char *replace , char *subject)
// ****************************************************************************
{
  return 0;
}

// ****************************************************************************
uint16_t portable_strpos(char *haystack, char *needle)
// ****************************************************************************
{
  char *    ptr           = haystack;
  uint8_t   len_haystack  = portable_strlen ( (char*) haystack );
  uint8_t   len_needle    = portable_strlen ( (char*) needle );
  int8_t    len_diff      = len_haystack - len_needle;
  uint8_t   buffer[c_big_buffer_size];
  uint16_t  l_res;

  if ( len_diff < 0 )
    return 0;

  for ( uint8_t j = 0; j < len_diff; j++ )
  {
    portable_strcpy_safe ( (char*) buffer, (char*)ptr, len_needle );
    l_res = portable_strcmp ( (char*) buffer, (char*) needle );
    if ( l_res == 0 )
      return j;
    ptr++;
  }
  return 0;
}

//  ****************************************************************************
uint16_t portable_strsplit (char* MyString, char** Items, char Delimiter, char EOT)
//  ****************************************************************************
{
  //
  //  result is an array of pointer which all point into the MyString area
  //
  uint8_t    OneChar;
  uint16_t  n;

  n = 0;
  *Items++ = MyString;

  do {

    OneChar = *MyString;                      //  Next char of string
    if ((OneChar == EOT) || (OneChar == 0))   //  is it the given or the absolute termination character
    {                                         //  then that s the end of text
      *MyString = 0;                          //  replace that char by \0
      return (n);                             //  return no of items
    }
    else
    if (OneChar == Delimiter)                 //  Is this the given delimiter
    {                                         //  yes
      n++;                                    //  increase the item count
      *MyString++ = 0;                        //  replace delimiter by \0
      *Items++ = MyString;                    //  save the pointer to the current item
    }
    else
    {
      MyString++;
    }

  } while (1);

  return (0);

} // str_split
// *****************************************************************************
// memcpy replacement
// *****************************************************************************
//
void portable_memcpy ( uint8_t * dest, uint8_t * source, uint32_t size )
{
  uint32_t i = 0;

  while ( i < size )
  {
    *(dest+i) = *(source+i);
    i++;
  }
  //i = 1;
}

// *****************************************************************************
// memset replacement
// *****************************************************************************
//
void portable_memset ( uint8_t * dest, uint8_t value, uint32_t size )
{
  uint32_t i = 0;

  while ( i < size )
  {
    *(dest+i) = value;
    i++;
  }
  //i = 1;
}

// *****************************************************************************
// memcmp replacement
// *****************************************************************************
//
int16_t  portable_memcmp ( uint8_t * dest, uint8_t * source, uint32_t size )
{
  int16_t  i       = 0;
  int16_t  result  = 0;

  while ( i < size )
  {
    if ( *(dest+i) != *(source+i) )
    {
      if ( *(dest+i) < *(source+i) )
        return -i-1;
      else
        return i+1;
    }
    i++;
  }
  return result;
}

// ****************************************************************************
// Converts an unsigned byte value to hex char
// ****************************************************************************
uint8_t byte_2_hex ( uint8_t byteval )
{
  if ( byteval <= 9U )
    return byteval + 48;
  else
    return byteval + 55;
}

// ****************************************************************************
// Converts an unsigned integer 32 word into a hex string
// ****************************************************************************
void portable_uint32_2_hex_str ( uint32_t n, char* res )
{
  //
  // uint32_t entspricht 4 bytes
  //
  uint32_t  my_n;
  uint8_t   ix, iy;

  memset ( (uint8_t*)res, 0, 4+2+1 );  // 0xnnnn\0
  my_n = n;

  for ( uint8_t i = 0; i < 4; i++ )
  {
    ix = ( my_n & 0x000f );
    iy = byte_2_hex ( ix );
    PreFix_Char ( iy, (char*)res );
    my_n  = my_n >> 4;
  }

  PreFix_Char ( 'x', (char*)res );
  PreFix_Char ( '0', (char*)res );

}

#define MEASURE_TIME 0
#if MEASURE_TIME
uint64_t getTime_usec(void);
COMMON uint64_t measure;
#endif

#if 0 // todo klaus fast version
/* ****************************************************************************
 Converts a floating-point/float number <n> to a string <res> on base 10
 with length <res_len> with given <no_of_decimals>
 padded with prefixed blanks to reach length "len"
 *****************************************************************************/
void portable_ftoa ( float _value, char* res, unsigned  no_of_decimals, unsigned res_len )
{
    float value = _value;
    ASSERT( no_of_decimals <= res_len-2);

#if MEASURE_TIME
    uint64_t start = getTime_usec();
#endif

    unsigned i=no_of_decimals;
    while( i-- > 0)
        value *=10.0f;

    int number;
    char sign;

    if( value < 0.0f)
    {
        sign = '-';
        number = (int)( -value + 0.5f);
    }
    else
        {
        sign = ' ';
        number = (int)( value + 0.5f);
        }

    char * target = res + res_len;
    *target-- = 0;

    for( i=no_of_decimals; i; --i)
    {
        *target-- = number % 10 + '0';
        number /= 10;
    }

    *target-- = '.';
    if( number == 0)
    {
        *target -- = '0';
    }
    else while(( number > 0) && ( target > res+1))
    {
        *target-- = number % 10 + '0';
        number /= 10;
    }

    *target-- = sign;

    while( target >= res)
        *target-- = ' ';

#if MEASURE_TIME
    measure -= measure >> 8;
    measure += getTime_usec() - start;
#endif
}

#else

//void portable_ftoa ( double n, char* res, uint8_t no_of_decimals, uint8_t res_len )
void portable_ftoa ( double n, char* res, unsigned  no_of_decimals, unsigned res_len )
{
#if MEASURE_TIME
    uint64_t start = getTime_usec();
#endif

  int8_t  sign = 1;     // Plus assumed
  int32_t  ipart;
  float    fpart;
  int32_t  i;
  uint8_t  len;
  float     xx = n;
  uint8_t loc_buffer[10] = {0};

// -------------------

  if ( isinf(n) ) my_break_point;
  //float  portable_round_2_n_decmls ( float Val1, int Val2 )


  memset ( (uint8_t *) res, 0, res_len + 1);            // preset output buffer

  if ( xx < 0 )                      // ixxput value is negative
  {
    sign  = -1;
    xx     = -xx;
  }

  fpart = 0.5;
  for ( i = 0; i < no_of_decimals; i++ )
    fpart = fpart / 10.0;
  xx += fpart;

  ipart   = (int)xx;                  // Extract integer part
  fpart   = xx - (float)ipart;            // Extract floating part
  len     = portable_itoa ( ipart, res, res_len );  // convert integer part to string

  // check for display option after point
  if (no_of_decimals != 0) {
    res[portable_strlen(res)] = '.';

    //  Get the value of fraction part up to given no. of decimals.
    //  The third parameter is needed to handle cases like 233.007
    //
    for ( i = 0; i < no_of_decimals; i++ )
      fpart = fpart * 10;
    //fpart += 0.5;
    //uint8_t ll = portable_strlen(res);

    len = portable_itoa ( (int)fpart, (char*) loc_buffer, 10 );  // convert integer part to string

    while ( ( no_of_decimals - len++) > 0 )
    {
      PreFix_Char ( '0', (char*) loc_buffer );
    }
  }
  strcat  ( (char*) res, (char*) loc_buffer );

  // if negative
  //
  if ( sign == -1 )              // if negative
  {
    PreFix_Char ( '-', res );
    len++;
  }

//  while ( portable_strlen ( res ) < len )
//  {
//    PreFix_Char ( ' ', res );
//  }
#if MEASURE_TIME
    measure -= measure >> 8;
    measure += getTime_usec() - start;
#endif
}

#endif

// ****************************************************************************
// portable_itoa2
//
//  CAUTION : Calling sequence different from stdlib itoa !!!!!!!!!!!!!!
//
// Converts an integer number <n> to a string <res> with given no of digits
//
//  CAUTION : Buffer res must have length <res_len> to contain all chars plus '-' sign
//
// ****************************************************************************
//
uint8_t portable_itoa2 ( int32_t n, char* res, uint8_t res_len, uint8_t prefix_pad_req, char prefix_pad_char )
{
  uint8_t   i     = 0;
  int8_t    sign  = 1;
  uint32_t  tmp;

  // ----------------------

  memset ( (uint8_t*) res, 0, res_len+1 );

  if ( n < 0 )                    // check if negative
  {
     n      = -n;                    // make positive
     sign   = -1;                    // remeber sign
  }

  tmp       = n;                    // count required digits, 1 is minimum
  while ( tmp > 0 )
  {
    tmp     = tmp / 10;
    i++;
  }

  if ( i > ( res_len ) )              // error condition
    memset ( (uint8_t * ) res, 'x', res_len );      // preset <res> with 'x'
  else
  {
    memset ( (uint8_t * ) res, 0, res_len );        // preset <res> with all zeros


    if ( n == 0 )                  // easy way out
       PreFix_Char ( '0', res );
    else
    {
      while ( n>0 )                // convert integer to string
      {
         tmp  = n;
         n    %= 10;
         tmp  /= 10;
         PreFix_Char ( ( n + '0'), res );
         n    = tmp;
      }
    }

    if ( sign == -1 )                // if negative, set prefix '-'
       PreFix_Char ( '-', res );

    if ( prefix_pad_req == 1 )
    {
      while ( portable_strlen(res) < res_len )
        PreFix_Char ( prefix_pad_char, res );
    }

  }
  return portable_strlen ( res );
}

// ****************************************************************************
// portable_itoahex
//
// Converts an integer number <n> to a hex string <res> with given no of digits
// ****************************************************************************
//
uint8_t portable_itoahex ( int32_t n, char* res, uint8_t res_len, uint8_t prefix_pad_req, char prefix_pad_char )
{
  uint8_t   i     = 0;
  uint32_t  tmp;

  // ----------------------

  memset ( (uint8_t*) res, 0, res_len+1 );

  if ( n < 0 )                    // check if negative
     n = -n;                    // make positive

  tmp = n;                    // count required digits, 1 is minimum
  while ( tmp > 0 )
  {
    tmp     = tmp / 16;
    i++;
  }

  if ( i > ( res_len ) )                            // error condition
    memset ( (uint8_t * ) res, 'x', res_len );      // preset <res> with 'x'
  else
  {
    memset ( (uint8_t * ) res, 0, res_len );        // preset <res> with all zeros

    if ( n == 0 )                  // special case -- easy way out
       PreFix_Char ( '0', res );
    else
    {
      while ( n > 0 )                // convert hex integer to string
      {
         tmp  = n;
         n    %= 16;
         tmp  /= 16;
         if ( n <= 9 )
           PreFix_Char ( ( n + '0'), res );
         else
           PreFix_Char ( ( n - 10 + 'A'), res );
         n    = tmp;
      }
    }

    if ( prefix_pad_req == 1 )
    {
      while ( portable_strlen(res) < res_len )
        PreFix_Char ( prefix_pad_char, res );
    }

  }
  return portable_strlen ( res );
}

// ****************************************************************************
// portable_itoa
//
// CAUTION : Calling sequence different from stdlib itoa !!!!!!!!!!!!!!
//
// Converts an integer number <n> to a string <res> with given no of digits
//
// CAUTION : Buffer res must have length <res_len> to contain all chars plus '-' sign
//
// ****************************************************************************
uint8_t portable_itoa ( int32_t n, char* res, uint8_t res_len )
{
  uint8_t   i    = 0;
  int8_t    sign  = 1;
  uint32_t  tmp;

  // ----------------------

  memset ( (uint8_t*) res, 0, res_len+1 );

  if ( n < 0 )                    // check if negative
  {
     n    = -n;                    // make positive
     sign  = -1;                    // remember sign
  }

  tmp    = n;                    // count required digits, 1 is minimum
  while ( tmp > 1 )
  {
    tmp  = tmp / 10;
    i++;
  }

  if ( i > ( res_len ) )              // error condition
    memset ( (uint8_t * ) res, 'x', res_len );      // preset <res> with 'x'
  else
  {
    memset ( (uint8_t * ) res, 0, res_len );        // preset <res> with all zeros


    if ( n == 0 )                  // easy way out
       PreFix_Char ( '0', res );
    else
    {
      while ( n>0 )                // convert integer to string
      {
         tmp  = n;
         n    %= 10;
         tmp  /= 10;
         PreFix_Char ( ( n + '0'), res );
         n    = tmp;
      }
    }

    if ( sign == -1 )                // if negative, set prefix '-'
       PreFix_Char ( '-', res );
  }
  return portable_strlen ( res );
}

// ****************************************************************************
int  portable_max ( int Val1, int Val2 )
// ****************************************************************************
{
  if (Val2 > Val1) {
    return(Val2);
  } else {
    return (Val1);
  }
 }
// portable_max


// ****************************************************************************
int  portable_min ( int Val1, int Val2 )
// ****************************************************************************
{
  if (Val2 < Val1) {
    return(Val2);
  } else {
    return (Val1);
  }
 }
// portable_max


// ****************************************************************************
float portable_fmax ( float Val1, float Val2 )
// ****************************************************************************
{
  if (Val2 > Val1) {
    return(Val2);
  } else {
    return (Val1);
  }
 }
// portable_max


// ****************************************************************************
float portable_fmin ( float Val1, float Val2 )
// ****************************************************************************
{
  if (Val2 < Val1) {
    return(Val2);
  }
  else {
    return (Val1);
  }
}
// portable_max



// ****************************************************************************
float    portable_fmod             ( float Val1, float Val2 )
// ****************************************************************************
{
  float x, y;
  x = Val1 / Val2;
  y = x * Val2;
  return (Val1 - y);
}



// ****************************************************************************
float  portable_round_2_n_decmls ( float Val1, int Val2 )
// ****************************************************************************
{
  float x = 1.0;
  float y;

  for ( uint8_t i = 0; i < Val2; i++ )
  {
    x *= 10.0;
  }
  if ( Val1 >= 0.0 )
    y = (int)( Val1 * x + 0.5 ) / x;
  else
    y = (int)( Val1 * x - 0.5 ) / x;
  return y;

 }
// portable_round_2_n_decmls


// ****************************************************************************
int32_t portable_round_2wards_zero ( float Val1 )
// ****************************************************************************
{
  int32_t l_x;
  int8_t  l_sign = sign_float ( Val1 );

  l_x = l_sign * floor ( fabs ( Val1 ) );
  return l_x;
 }
// portable_round_2wards_zero


// ****************************************************************************
int    sign_int            ( int value )
// ****************************************************************************
{
  return ( ( value > 0.0 ) ? 1 : -1 );
}


// ****************************************************************************
int    sign_long            ( long value )
// ****************************************************************************
{
  return ( ( value > 0.0 ) ? 1 : -1 );
}

// ****************************************************************************
int    sign_float          ( float value )
// ****************************************************************************
{
  return ( ( value > 0.0 ) ? 1 : -1 );
}

// ****************************************************************************
int    sign_double          ( double value )
// ****************************************************************************
{
  return ( ( value > 0.0 ) ? 1 : -1 );
}

// ****************************************************************************
float  portable_safe_tan ( float Val )
// ****************************************************************************
{
  float x = fabs ( fabs ( Val ) - c_PiD2_90Grad );
  if ( x < c_Jota )
  {
    if ( Val >= 0.0 )
      return 1.0 / c_Jota;
    else
      return -1.0 / c_Jota;
  }
  else
    return sinf ( Val ) / cosf ( Val );
}


// ****************************************************************************
float  portable_atof ( const char * ValText )
// ****************************************************************************
{
  // + or -, digits, with 0 or 1 periods in it.

  uint16_t  decimals  = 0;
  uint32_t  i         = 0;
  uint8_t   got_dot   = 0;    // not dot assumed
  int8_t    sign      = 1;    // plus assumed
  float     result    = 0;

  //
  // delete all blanks from string
  //
  portable_compress ( (char*)ValText );

  //
  //  check special cases
  //
  while ( ValText[0] == '.' )
  {
    got_dot = 1;
    portable_trimleading ( (char*) ValText, '.' );
  }

  while ( ValText[0]== '+' )
  {
    portable_trimleading ( (char*) ValText, '+' );
  }

  while ( ValText[0]== '-' )
  {
    sign = -sign;
    portable_trimleading ( (char*) ValText, '-' );
  }

  //
  //  normal processing
  //
  for ( i = 0; i < portable_strlen((char*)ValText); i++ )
  {
    if ( ValText[i] == '.' )
    {
        got_dot = 1;
    }
    else
    if ( portable_isdigit ( (char)  ValText[i]) )
    {
      //  before dot
      if (got_dot == 1 )
        decimals++;
      result = result * 10 + ( ValText[i] - '0' );
    }
  }

  while (decimals-- > 0)
    result = result / 10;

  result  = sign * result;

  return result;
}


#define uint32_t_max  0xFFFFFFFF
#define uint16_t_max  0x7FFFFFFF

// ****************************************************************************
int16_t portable_atoi ( const char* ValText )
// ****************************************************************************
{
  uint32_t  i         = 0;
  uint8_t   sign      = 1;    // plus assumed
  int16_t   result    = 0;
  //
  //  normal processing
  //
  portable_compress ( (char*)ValText );

  //
  //  check special cases
  //
  while ( ValText[0]== '+' )
  {
    portable_trimleading ( (char*) ValText, '+' );
  }

  while ( ValText[0]== '-' )
  {
    sign = -sign;
    portable_trimleading ( (char*) ValText, '-' );
  }

  //
  //  normal processing
  //
  for ( i = 0; i < portable_strlen((char*)ValText); i++ )
  {
    if ( ValText[i] == '.' )
      return result;  // no dot in integer
    else if ( ValText[i] == '+' )
      return result;  // no second +
    else if ( ValText[i] == '-' )
      return result;  // no second -
    else if ( portable_isdigit ( (char)  ValText[i]) )
      result = result * 10 + ( ValText[i] - '0' );
  }

  result  = sign * result;

  return result;

} // portable_atoi

// ****************************************************************************
int32_t portable_atol ( const char* ValText )
// ****************************************************************************
{
  uint32_t  i         = 0;
  uint8_t   sign      = 1;    // plus assumed
  int32_t   result    = 0;
  //
  //  normal processing
  //
  portable_compress ( (char*)ValText );

  //
  //  check special cases
  //
  while ( ValText[0]== '+' )
  {
    portable_trimleading ( (char*) ValText, '+' );
  }

  while ( ValText[0]== '-' )
  {
    sign = -sign;
    portable_trimleading ( (char*) ValText, '-' );
  }

  //
  //  normal processing
  //
  for ( i = 0; i < portable_strlen((char*)ValText); i++ )
  {
    if ( ValText[i] == '.' )
      return result;  // no dot in integer
    else if ( ValText[i] == '+' )
      return result;  // no second +
    else if ( ValText[i] == '-' )
      return result;  // no second -
    else if ( portable_isdigit ( (char)  ValText[i]) )
      result = result * 10 + ( ValText[i] - '0' );
  }

  result  = sign * result;

  return result;

} // portable_atol

// ****************************************************************************
// ARMA filter 2nd order    c/o Dr. Klaus Schaefer
// ****************************************************************************
//

float ARMA2O ( float input )
{
  #define Koeff_1  (0.136728735)
  #define Koeff_2  (0.726542528)

  float  old_input = 0.0;
  float  old_output = 0.0;
  float  output;

  output = ( old_input + input ) * Koeff_1 + old_output * Koeff_2;
  old_output = output;
  old_input = input;
  return output;
}


// *****************************************************************************
// decimal to binary string
// *****************************************************************************
//
uint8_t * portable_Decimal_2_Binary ( uint32_t Data, uint8_t NoOfBits, uint8_t * out_buffer )
{
  uint8_t *   MyPtr = out_buffer;
  uint32_t    Mask;
  uint8_t     i;

  Mask = 1L << (NoOfBits-1);

  for (i = NoOfBits ; i > 0 ; i--)
  {
    if (Data & Mask)
      *MyPtr++ = '1';
    else
      *MyPtr++ = '0';

    Mask = Mask >> 1;

    if ( ( ( i & 3 ) == 1 ) && ( i > 1 ) )
    {
      *MyPtr++ = '.';
    }
  }

  *MyPtr = '\0';

  return MyPtr;

}

// *****************************************************************************
// decimal to binary string
// *****************************************************************************
//
uint8_t * portable_Decimal_2_Binary_Color_Mask ( uint16_t Data, uint8_t * out_buffer )
{
  uint8_t     NoOfBits = 16;
  uint8_t *   MyPtr = out_buffer;
  uint32_t    Mask;
  uint8_t     i;

  Mask = 1L << (NoOfBits-1);

  for (i = NoOfBits; i > 0 ; i--)
  {
    if ( ( i % 5 ) == 0 )
      *MyPtr++ = '.';

    if (Data & Mask)
      *MyPtr++ = '1';
    else
      *MyPtr++ = '0';

    Mask = Mask >> 1;
  }

  *MyPtr = '\0';

  return MyPtr;

}

/// ****************************************************************************
// Binary BCD Conversion
// ****************************************************************************
uint8_t  portable_Binary_2_BCD ( uint8_t input )
// ****************************************************************************
{
  uint8_t    hi_nibble = 0;
  uint8_t    lo_nibble;

  lo_nibble = input;
  while ( lo_nibble > 10 )
  {
    lo_nibble = lo_nibble - 10;
    hi_nibble++;
  }
  return ( ( hi_nibble << 4 ) + lo_nibble );

}

// ****************************************************************************
// BCD Binary Conversion
// ****************************************************************************
uint8_t  portable_BCD_2_Binary ( uint8_t input )
{
  uint8_t    hi_nibble;
  uint8_t    lo_nibble;

  hi_nibble = input >> 4;
  lo_nibble = input & 0x0F;

  return ( hi_nibble * 10 + lo_nibble );

}

// ****************************************************************************
// convert latitude in RAD to string
// ****************************************************************************
//
void lat_rad_2_NS_DDMMSS  ( float latitude_rad, uint8_t * buffer )
{
  char  l_NS;
  //uint8_t  l_buffer[c_len_coord_buffer];

  if ( latitude_rad > 0 )
  {
    l_NS = 'N';
  } else {
    l_NS = 'S';
  }

  float l_deg = latitude_rad * 180.0 / c_Pi_180Grad;
  int8_t  l_units  = floor ( l_deg );
  l_deg -= l_units;
  l_deg *= 60.0;
  uint8_t  l_minutes = floor ( l_deg );
  l_deg -= l_minutes;
  l_deg *= 60.0;
  uint8_t  l_seconds = floor ( l_deg );

  memset ( (uint8_t * ) buffer, 0, c_len_coord_buffer );
  PreFix_Char_Bounded ( l_NS, (char*)buffer, c_len_coord_buffer );
  uint8_t ll = portable_strlen( (const char * ) buffer);
  portable_itoa2 ( l_units, (char*) &buffer[ll], 3, 1, '0' );
  PostFix_Char_Bounded ( ':', (char*)buffer, c_len_coord_buffer );
  ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_minutes, (char*) &buffer[ll], 2, 1, '0' );
  PostFix_Char_Bounded ( ':', (char*)buffer, c_len_coord_buffer );
  ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_seconds, (char*) &buffer[ll], 2, 1, '0' );
}

// ****************************************************************************
// convert longitude in RAD to string
// ****************************************************************************
//
void lon_rad_2_EW_DDDMMSS  ( float longitude_rad, uint8_t * buffer )
{
  uint8_t  l_EW;

  if ( longitude_rad > 0 )
  {
    l_EW = 'E';
  } else {
    l_EW = 'W';
  }

  float l_deg = longitude_rad * 180.0 / c_Pi_180Grad;
  int8_t  l_units  = floor ( l_deg );
  l_deg -= l_units;
  l_deg *= 60.0;
  uint8_t  l_minutes = floor ( l_deg );
  l_deg -= l_minutes;
  l_deg *= 60.0;
  uint8_t  l_seconds = floor ( l_deg );

  memset ( (uint8_t * ) buffer, 0, c_len_coord_buffer );
  PreFix_Char_Bounded ( l_EW, (char*)buffer, c_len_coord_buffer );
  uint8_t ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_units, ( char * ) &buffer[ll], 3, 1, '0' );
  PostFix_Char_Bounded ( ':', (char*)buffer, c_len_coord_buffer );
  ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_minutes, ( char * ) &buffer[ll], 2, 1, '0' );
  PostFix_Char_Bounded ( ':', (char*)buffer, c_len_coord_buffer );
  ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_seconds, ( char * ) &buffer[ll], 2, 1, '0' );
}

// ****************************************************************************
// convert latitude in decimal degrees to string
// ****************************************************************************
//
void  lat_dec_deg_2_NS_DDMMSS   ( float latitude_deg, uint8_t * buffer )
{
  uint8_t  l_NS;

  if ( latitude_deg > 0 )
  {
    l_NS = 'N';
  } else {
    l_NS = 'S';
  }

  float l_deg = latitude_deg;
  int8_t  l_units  = floor ( l_deg );
  l_deg -= l_units;
  l_deg *= 60.0;
  uint8_t  l_minutes = floor ( l_deg );
  l_deg -= l_minutes;
  l_deg *= 60.0;
  uint8_t  l_seconds = floor ( l_deg );

  memset ( (uint8_t * ) buffer, 0, c_len_coord_buffer );
  PreFix_Char_Bounded ( l_NS, (char*)buffer, c_len_coord_buffer );
  uint8_t ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_units, ( char * ) &buffer[ll], 3, 1, '0' );
  PostFix_Char_Bounded ( ':', (char*)buffer, c_len_coord_buffer );
  ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_minutes, ( char * ) &buffer[ll], 2, 1, '0' );
  PostFix_Char_Bounded ( ':', (char*)buffer, c_len_coord_buffer );
  ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_seconds, ( char * ) &buffer[ll], 2, 1, '0' );
}

// ****************************************************************************
// convert longitude in decimal degrees to string
// ****************************************************************************
//
void  lon_dec_deg_2_EW_DDDMMSS   ( float longitude_deg, uint8_t * buffer )
{
  uint8_t  l_EW;

  if ( longitude_deg > 0 )
  {
    l_EW = 'E';
  } else {
    l_EW = 'W';
  }

  float l_deg = longitude_deg;
  int8_t  l_units  = floor ( l_deg );
  l_deg -= l_units;
  l_deg *= 60.0;
  uint8_t  l_minutes = floor ( l_deg );
  l_deg -= l_minutes;
  l_deg *= 60.0;
  uint8_t  l_seconds = floor ( l_deg );

  memset ( (uint8_t * ) buffer, 0, c_len_coord_buffer );
  PreFix_Char_Bounded ( l_EW, (char*)buffer, c_len_coord_buffer );
  uint8_t ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_units, ( char * ) &buffer[ll], 3, 1, '0' );
  PostFix_Char_Bounded ( ':', (char*)buffer, c_len_coord_buffer );
  ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_minutes, ( char * ) &buffer[ll], 2, 1, '0' );
  PostFix_Char_Bounded ( ':', (char*)buffer, c_len_coord_buffer );
  ll = portable_strlen((const char * )buffer);
  portable_itoa2 ( l_seconds, ( char * ) &buffer[ll], 2, 1, '0' );
}

// ****************************************************************************
// convert latitude in NMEA formatted string
//
//  includes trailing comma
//
// ****************************************************************************
//
void  lat_2_NMEA_string ( float latitude_deg, uint8_t * loc_string )
{
  uint8_t l_NS;
  uint8_t buffer[5];
  float   f_loc_lat;

  f_loc_lat = latitude_deg;
  if ( f_loc_lat > 0 )     // north counted positive / south counted negative
  {
    l_NS = 'N';
  } else {
    l_NS = 'S';
    f_loc_lat = -f_loc_lat;
  }

  uint16_t  l_lat_deg     = floor ( f_loc_lat );                    // the integer part
  float     f_lat_min     = ( f_loc_lat - l_lat_deg ) * 60.0;             // the remainder part = decimal minutes

  uint16_t  l_lat_min     = floor ( f_lat_min );                  // minutes the integer part
  float_t   f_lat_sec     = ( f_lat_min - l_lat_min ) * 100.0;     // the remainder part = decimal seconds

  uint16_t  l_lat_sec     = floor ( f_lat_sec * 10.0 + 0.5 );


  //
  //  Worst case output is "DDMM.SSS,S," = 11 chars + NUL.
  //
  #define c_lat_2_NMEA_MaxLen  16

  portable_itoa2 ( l_lat_deg, (char*) buffer, 2, true, '0' );     // 2 digits, leading zero
  portable_strcpy ( (char*) loc_string, (const char*) buffer );

  portable_itoa2 ( l_lat_min, (char*) buffer, 2, true, '0' );     // 2 digits, leading zero
  portable_strcat_bounded ( (char*) loc_string, (const char*) buffer, c_lat_2_NMEA_MaxLen );

  PostFix_Char_Bounded ( '.', (char*) loc_string, c_lat_2_NMEA_MaxLen );
  
  portable_itoa2 ( l_lat_sec, (char*) buffer, 3, true, '0' );     // 3 digits, leading zero
  portable_strcat_bounded ( (char*) loc_string, (const char*) buffer, c_lat_2_NMEA_MaxLen );


  PostFix_Char_Bounded ( ',', (char*) loc_string, c_lat_2_NMEA_MaxLen );
  
  PostFix_Char_Bounded ( l_NS, (char*) loc_string, c_lat_2_NMEA_MaxLen );                      // southern or northern hemisphere

  PostFix_Char_Bounded ( ',', (char*) loc_string, c_lat_2_NMEA_MaxLen );

  #undef c_lat_2_NMEA_MaxLen
}

// ****************************************************************************
// convert longitude in NMEA formatted string
//
//  includes trailing comma
//
// ****************************************************************************
//
void  lon_2_NMEA_string ( float longitude_deg, uint8_t * loc_string )
{
  uint8_t  l_EW;
  uint8_t buffer[5];
  float   f_loc_lon;

  f_loc_lon = longitude_deg;
  if ( f_loc_lon > 0 )     // north counted positive / south counted negative
  {
    l_EW = 'E';
  } else {
    l_EW = 'W';
    f_loc_lon = -f_loc_lon;
  }

  uint16_t  l_lon_deg     = floor ( f_loc_lon );                    // the integer part
  float     f_lon_min     = ( f_loc_lon - l_lon_deg ) * 60.0;       // the remainder part = decimal minutes

  uint16_t  l_lon_min     = floor ( f_lon_min );                  // minutes the integer part
  float     f_lon_sec     = ( f_lon_min - l_lon_min ) * 100.0;     // the remainder part = decimal seconds

  uint16_t  l_lon_sec     = floor ( f_lon_sec * 10.0 + 0.5 );


  //
  //  Worst case output is "DDDMM.SSS,E," = 12 chars + NUL.
  //
  #define c_lon_2_NMEA_MaxLen  16

  portable_itoa2 ( l_lon_deg, (char*) buffer, 3, true, '0' );     // 3 digits, leading zero
  portable_strcpy ( (char*) loc_string, (const char*) buffer );

  portable_itoa2 ( l_lon_min, (char*) buffer, 2, true, '0' );     // 2 digits, leading zero
  portable_strcat_bounded ( (char*) loc_string, (const char*) buffer, c_lon_2_NMEA_MaxLen );


  PostFix_Char_Bounded ( '.', (char*) loc_string, c_lon_2_NMEA_MaxLen );
  
  portable_itoa2 ( l_lon_sec, (char*) buffer, 3, true, '0' );     // 3 digits, leading zero
  portable_strcat_bounded ( (char*) loc_string, (const char*) buffer, c_lon_2_NMEA_MaxLen );

  PostFix_Char_Bounded ( ',', (char*) loc_string, c_lon_2_NMEA_MaxLen );
  
  PostFix_Char_Bounded ( l_EW, (char*) loc_string, c_lon_2_NMEA_MaxLen );                      // east or west hemisphere
  
  PostFix_Char_Bounded ( ',', (char*) loc_string, c_lon_2_NMEA_MaxLen );

  #undef c_lon_2_NMEA_MaxLen
}

// ****************************************************************************
// offset coordinates
// ****************************************************************************
//
void OffsetCoordinates ( point_i16_t * Offset ,
                         point_i16_t * StartPoint
                )
{
  StartPoint->x += Offset->x;
  StartPoint->y += Offset->y;
}

// ****************************************************************************
//  rotation of a vector inside a fixed coord system
// ****************************************************************************
//
void RotateFlatCoordinates (  point_i16_t * StartPoint,
                              point_i16_t * TargetPoint,
                              double _angle )
{
    float angle = _angle;
  float y_cos, y_sin, x, y;

  // --------------------------------------------------------------------

  y_cos = cosf ( angle );
  y_sin = sinf ( angle );
  x = y_cos * StartPoint->x + y_sin * StartPoint->y;
  y = y_cos * StartPoint->y - y_sin * StartPoint->x;

  TargetPoint->x = portable_round_2_n_decmls ( x, 0 );
  TargetPoint->y = portable_round_2_n_decmls ( y, 0 ) ;

//  TargetPoint->x = portable_round_2wards_zero ( x );
//  TargetPoint->y = portable_round_2wards_zero ( y );

  return;
}

// ***********************************************************************
// ***********************************************************************
//
// ****************************************************************************
//  own time notation handling
//
//  compact date time elements into a 32 bit word
// ****************************************************************************
//
uint32_t  SetDateTime     ( int16_t year, int16_t mon, int16_t day,
                            int16_t hour, int16_t min, int16_t sec )
{
  uint32_t  x = year;
  x = ( x << 4 ) + mon;     // 12 --> 16 --> 4 bit
  x = ( x << 5 ) + day;     // 32 --> 32 --> 5 bit
  x = ( x << 5 ) + hour;    // 24 --> 32 --> 5 bit
  x = ( x << 6 ) + min;     // 60 --> 64 --> 6 bit
  x = ( x << 6 ) + sec;     // 60 --> 64 --> 6 bit
  return x;
}

// ****************************************************************************
//  translate UTC time into local time using an hourly offset
//  This routine is leapyear aware up to the next centenial
// ****************************************************************************
//
uint32_t  MakeLOCFromUTCTime  ( uint32_t GPS_date_time, int8_t  UTC_Offset )
{
  int16_t  year;
  int16_t  month;
  int16_t  day;
  int16_t  hour;
  int16_t  minute;
  int16_t  second;
  uint32_t  LOC_date_time;
  uint8_t   DaysInThisMonth;
  uint8_t   DaysInPrevMonth;
  int8_t    Day_Offset = 0;

  int8_t   DaysInMonthNormalYear[12] =
                // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
                  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  int8_t   DaysInMonthLeapYear  [12] =
                // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
                  { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  DateTimeReConvert ( GPS_date_time,
                      (int16_t*)&year, (int16_t*)&month,  (int16_t*)&day,
                      (int16_t*)&hour, (int16_t*)&minute, (int16_t*)&second );

  hour = hour + UTC_Offset;
  if ( hour < 0 )
  {
    hour +=24;
    Day_Offset = -1;
  }
  if ( hour >= 24 )
  {
    Day_Offset = 1;
    hour -= 24;
  }

  if ( year / 4 * 4  == year )
  {
    DaysInPrevMonth = DaysInMonthLeapYear[( month-2 ) % 12];
    DaysInThisMonth = DaysInMonthLeapYear[( month-1 ) % 12];
  }
  else
  {
    DaysInPrevMonth = DaysInMonthNormalYear[( month-2 ) % 12];
    DaysInThisMonth = DaysInMonthNormalYear[( month-1 ) % 12];
  }

  day += Day_Offset;

  if ( day < 0 )
  {
    day = DaysInPrevMonth;
    month--;
    if ( month < 0 )
    {
      month = 12;
      year--;
    }
  }

  if ( day > DaysInThisMonth )
  {
    day = 1;
    month++;
    if ( month > 12 )
    {
      month = 1;
      year++;
    }
  }


  LOC_date_time = SetDateTime ( year, month, day, hour, minute, second );
  return    LOC_date_time;
}

// ****************************************************************************
//  Reconvert packed time from int32_t word into string
//  selector = 0 full string, selector = 1 data only, selector = 2 time only
// ***********************************************************************
//
void DateTimeToString ( uint8_t * timestring, uint32_t date_time, uint8_t selector, uint8_t NoDelimiters )
{
  int16_t  year;
  int16_t  month;
  int16_t  day;
  int16_t  hour;
  int16_t  minute;
  int16_t  second;
  uint8_t   l_buffer[5];
  //
  //  Worst case output is "YYYY-MM-DD HH:MM:SS" = 19 chars + NUL.
  //  This is a defensive cap on what THIS function will ever write,
  //  independent of whatever size buffer the caller actually passed in.
  //
  #define c_DateTimeToString_MaxLen  20

  DateTimeReConvert(
      date_time,
      (int16_t *)&year, (int16_t *)&month,  (int16_t *)&day,
      (int16_t *)&hour, (int16_t *)&minute, (int16_t *)&second);

  timestring[0] = '\0';

  if ( ( selector == 0 ) || ( selector == 1 ) )
  {
    portable_itoa2(year + 2000, (char*) &l_buffer, 4, 1, '0');
    portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_DateTimeToString_MaxLen);
    if ( ! NoDelimiters )
      PostFix_Char_Bounded ( '-', (char*) timestring, c_DateTimeToString_MaxLen );

    portable_itoa2(month, (char*) &l_buffer, 2, 1, '0');
    portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_DateTimeToString_MaxLen);
    if ( ! NoDelimiters )
      PostFix_Char_Bounded ( '-', (char*) timestring, c_DateTimeToString_MaxLen );

    portable_itoa2(day, (char*) &l_buffer, 2, 1, '0');
    portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_DateTimeToString_MaxLen);
    if ( ! NoDelimiters )
      PostFix_Char_Bounded (' ', (char*) timestring, c_DateTimeToString_MaxLen );
  }

  if ( ( selector == 0 ) || ( selector == 2 ) )
  {
    portable_itoa2(hour, (char*) &l_buffer, 2, 1, '0');
    portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_DateTimeToString_MaxLen);
    if ( ! NoDelimiters )
      PostFix_Char_Bounded ( ':', (char*) timestring, c_DateTimeToString_MaxLen );

    portable_itoa2(minute, (char*) &l_buffer, 2, 1, '0');
    portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_DateTimeToString_MaxLen);
    if ( ! NoDelimiters )
      PostFix_Char_Bounded ( ':', (char*) timestring, c_DateTimeToString_MaxLen );

    portable_itoa2(second, (char*) &l_buffer, 2, 1, '0');
    portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_DateTimeToString_MaxLen);
  }

  #undef c_DateTimeToString_MaxLen
}

// ****************************************************************************
//  Create NMEA time stamp (without fractional seconds)
//  Trailing comma attached
// ***********************************************************************
//
void NMEA_TimeStamp ( uint8_t * timestring, uint32_t date_time )
{
  int16_t  year;
  int16_t  month;
  int16_t  day;
  int16_t  hour;
  int16_t  minute;
  int16_t  second;
  uint8_t  l_buffer[5];
  //
  //  Worst case output is "HHMMSS.00," = 10 chars + NUL.
  //
  #define c_NMEA_TimeStamp_MaxLen  16

  DateTimeReConvert(
      date_time,
      (int16_t *)&year, (int16_t *)&month,  (int16_t *)&day,
      (int16_t *)&hour, (int16_t *)&minute, (int16_t *)&second);

  timestring[0] = '\0';

  portable_itoa2(hour, (char*) &l_buffer, 2, 1, '0');
  portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_NMEA_TimeStamp_MaxLen);

  portable_itoa2(minute, (char*) &l_buffer, 2, 1, '0');
  portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_NMEA_TimeStamp_MaxLen);

  portable_itoa2(second, (char*) &l_buffer, 2, 1, '0');
  portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_NMEA_TimeStamp_MaxLen);

  portable_strcat_bounded ( (char*) timestring, (char*) ".00,", c_NMEA_TimeStamp_MaxLen);
  //PostFix_Char ( ',', (char*) timestring );

  #undef c_NMEA_TimeStamp_MaxLen
}

// ****************************************************************************
//  Create NMEA date stamp
//  Trailing comma attached
// ***********************************************************************
//
void NMEA_DateStamp ( uint8_t * timestring, uint32_t date_time )
{
  int16_t  year;
  int16_t  month;
  int16_t  day;
  int16_t  hour;
  int16_t  minute;
  int16_t  second;
  uint8_t  l_buffer[5];
  //
  //  Worst case output is "YYMMDD," = 7 chars + NUL.
  //
  #define c_NMEA_DateStamp_MaxLen  12

  DateTimeReConvert(
      date_time,
      (int16_t *)&year, (int16_t *)&month,  (int16_t *)&day,
      (int16_t *)&hour, (int16_t *)&minute, (int16_t *)&second);

  timestring[0] = '\0';

  portable_itoa2(year, (char*) &l_buffer, 2, 1, '0');
  portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_NMEA_DateStamp_MaxLen);

  portable_itoa2(month, (char*) &l_buffer, 2, 1, '0');
  portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_NMEA_DateStamp_MaxLen);

  portable_itoa2(day, (char*) &l_buffer, 2, 1, '0');
  portable_strcat_bounded ( (char*) timestring, (char*) &l_buffer, c_NMEA_DateStamp_MaxLen);

  PostFix_Char_Bounded ( ',', (char*) timestring, c_NMEA_DateStamp_MaxLen );

  #undef c_NMEA_DateStamp_MaxLen
}

// ****************************************************************************
//  Reconvert packed time from int32_t word into atoms
// ***********************************************************************
//

void DateTimeReConvert  ( uint32_t date_time,
                          int16_t * year, int16_t * month,  int16_t * day,
                          int16_t * hour, int16_t * minute, int16_t * second )
{
  uint32_t  l_x;

  l_x = date_time;
  *second = l_x & 0x3F;
  l_x = l_x >> 6;

  *minute = l_x & 0x3F;
  l_x = l_x >> 6;

  *hour = l_x & 0x1F;
  l_x = l_x >> 5;

  *day = l_x & 0x1F;
  l_x = l_x >> 5;

  *month = l_x & 0xF;
  l_x = l_x >> 4;

  *year  = l_x;

}

// ****************************************************************************
//  Diff of two date_time variables
// ***********************************************************************
//
uint32_t  Portable_IntraDay_TimeDiff ( uint32_t date_time_2, uint32_t date_time_1 )
{
  int16_t year2;
  int16_t month2;
  int16_t day2;
  int16_t hour2;
  int16_t minute2;
  int16_t second2;
  int16_t year1;
  int16_t month1;
  int16_t day1;
  int16_t hour1;
  int16_t minute1;
  int16_t second1;
  uint32_t  result;

  DateTimeReConvert ( date_time_2, &year2, &month2, &day2, &hour2, &minute2, &second2 );
  DateTimeReConvert ( date_time_1, &year1, &month1, &day1, &hour1, &minute1, &second1 );

  if ( date_time_1 < date_time_2 )
  {

    second2 = second2   - second1;
    if ( second2 < 0 )
    {
      second2 += 60;
      minute1++;
    }

    minute2 = minute2  - minute1;
    if ( minute2 < 0 )
    {
      minute2 += 60;
      hour1++;
    }

    hour2   = hour2     - hour1;
    year2   = 0;
    month2  = 0;
    day2    = 0;

    result = SetDateTime ( year2, month2, day2, hour2, minute2, second2 );
  }
  else
    result = SetDateTime ( year2, month2, day2, 0, 0, 0 );

  return result;
}

// ****************************************************************************
// Helper function for simple filter
// ****************************************************************************
//
float Calc_Tau ( float tau, uint16_t  rep_period )
{
  if ( tau == 0.0 )
    return 1.0;
  else
    return ( (float) rep_period / ( 1000.0 * tau ) );
}

// *****************************************************************************
// portable_strrchr - find LAST occurrence of a character in a string
// *****************************************************************************
//
char * portable_strrchr ( char * p_str, char p_char )
{
  int16_t  i;
  uint16_t l_len = portable_strlen ( p_str );

  for ( i = ( (int16_t) l_len - 1 ); i >= 0; i-- )
  {
    if ( p_str[i] == p_char )
    {
      return &p_str[i];
    }
  }
  return NULL;
}


// ****************************************************************************
// End of File
// ****************************************************************************