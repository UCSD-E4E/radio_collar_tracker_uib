#include <stdio.h>
#include <string.h>

char* strtok_e(char *str, char delim)
{
  static char* store_i;
  int i;
  if(str == NULL)
  {
    str = store_i;
    for(i = 0; str[i] != '\0'; i++)
    {
      if(str[i] == delim || str[i] == '*')
      {
        str[i] = '\0';
        char* temp_i = store_i;
        store_i = &str[i + 1];
        if(temp_i[0] == '\0')
        {
          return NULL;
        }
        else
        {
          return temp_i;
        }
      }
    }
  }

  /* first use of strtok_e */
  else
  {
    for(i = 0; str[i] != '\0'; i++)
    {
      if(str[i] == delim || str[i] == '*')
      {
        str[i] = '\0';
        store_i = &str[i + 1];
        if(str[0] == '\0')
        {
          return NULL;
        }
        else
        {
          return &str[0];
        }
      }
    }
  }
  return NULL;
}