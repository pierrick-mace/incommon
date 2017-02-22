#ifndef __DEFINES_H__
#define __DEFINES_H__

#define STRLEN_MAX 31
#define MAX_SEPARATORS 6

#define ERROR error = true; \
              goto error
              
#define IS_NULL(expr, ret) if(expr == NULL) { \
                                return ret; \
                            }
                      
#define IS_NULL_INST(expr, inst, ret) if(expr == NULL) { \
                                          inst; \
                                          return ret; \
                                        } \
                                  
#define IF_ERROR(expr, err) if(expr == err) { \
                                return err; \
                            }
                            
#define IF_ERROR_INST(expr, inst, err) if(expr == err) { \
                                            inst; \
                                            return err; \
                                       } 

#endif
