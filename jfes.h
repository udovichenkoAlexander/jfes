/**
    \file       jfes.h
    \author     Eremin Dmitry (http://github.com/NeonMercury)
    \date       October, 2015
    \brief      Json For Embedded Systems library headers.
*/

#ifndef JFES_H_INCLUDE_GUARD
#define JFES_H_INCLUDE_GUARD

/** Strict JSON mode. **/
//#define JFES_STRICT

/** Maximal tokens count */
#define JFES_MAX_TOKENS_COUNT   8192

/** NULL define for the jfes library. */
#ifndef JFES_NULL
#define JFES_NULL               ((void*)0)
#endif

/** size_t type for the jfes library. */
typedef unsigned int jfes_size_t;

/** JFES return statuses. */
typedef enum jfes_status {
    jfes_unknown            = 0x00,             /**< Unknown status */

    jfes_success            = 0x01,             /**< Last operation finished sucessfully. */

    jfes_invalid_arguments  = 0x02,             /**< Invalid arguments were passed to the function. */
    jfes_no_memory          = 0x03,             /**< Not enough tokens were provided. */
    jfes_invalid_input      = 0x04,             /**< Invalid character in JSON string. */
    jfes_error_part         = 0x05,             /**< The string is not a full JSON packet. More bytes expected. */
    jfes_unknown_type       = 0x06,             /**< Unknown token type. */
} jfes_status_t;

/** Memory allocator function type. */
typedef void *(__cdecl *jfes_malloc_t)(jfes_size_t);

/** Memory deallocator function type. */
typedef void (__cdecl *jfes_free_t)(void*);

/** JFES string type. */
typedef struct jfes_string {
    char            *data;                      /**< String bytes. */
    jfes_size_t     size;                       /**< Allocated bytes count. */
} jfes_string_t;

/** JFES token types */
typedef enum jfes_token_type {
    jfes_undefined          = 0x00,             /**< Undefined token type. */
    
    jfes_boolean            = 0x01,             /**< Boolean token type. */
    jfes_integer            = 0x02,             /**< Integer token type. */
    jfes_double             = 0x03,             /**< Double token type. */
    jfes_string             = 0x04,             /**< String token type. */

    jfes_array              = 0x05,             /**< Array token type. */
    jfes_object             = 0x06              /**< Object token type. */
} jfes_token_type_t;

/** Json value type is the same as token type. */
typedef jfes_token_type_t jfes_value_type_t;

/** JFES token structure. */
typedef struct jfes_token {
    jfes_token_type_t       type;               /**< Token type. */

    int                     start;              /**< Token start position. */
    int                     end;                /**< Token end position. */
    jfes_size_t             size;               /**< Token children count. */
} jfes_token_t;

/** JFES config structure. */
typedef struct jfes_config {
    jfes_malloc_t           jfes_malloc;        /**< Memory allocation function. */
    jfes_free_t             jfes_free;          /**< Memory deallocation function. */
} jfes_config_t;

/** JFES tokens data structure. */
typedef struct jfes_tokens_data {
    jfes_config_t           *config;            /**< JFES configuration. */

    const char              *json_data;         /**< JSON string. */
    jfes_size_t             json_data_length;   /**< JSON string length. */

    jfes_token_t            *tokens;            /**< String parsing result in tokens. */
    jfes_size_t             tokens_count;       /**< Tokens count. */
    jfes_size_t             current_token;      /**< Index of current token. */
} jfes_tokens_data_t;

/** JFES parser structure. */
typedef struct jfes_parser {
    jfes_size_t             pos;                /**< Current offset in json string. */ 
    jfes_size_t             next_token;         /**< Next token to allocate. */
    int                     superior_token;     /**< Superior token node. */

    jfes_config_t           *config;            /**< Pointer to jfes config. */
} jfes_parser_t;


/** JSON value structure. */
typedef struct jfes_value jfes_value_t;

/** JFES `key -> value` mapping structure. */
typedef struct jfes_object_map {
    jfes_string_t           key;                /**< Object key. */
    jfes_value_t            *value;             /**< Oject value. */
} jfes_object_map_t;

/** JSON array structure. */
typedef struct jfes_array {
    jfes_value_t            **items;            /**< JSON items in array. */    
    jfes_size_t             count;              /**< Items count in array. */
} jfes_array_t;

/** JSON object structure. */
typedef struct jfes_object {
    jfes_object_map_t       **items;            /**< JSON items in object. */
    jfes_size_t             count;              /**< Items count in object. */
} jfes_object_t;

/** JFES value data union. */
typedef union jfes_value_data {
    int                     bool_val;           /**< Boolean JSON value. */

    int                     int_val;            /**< Integer JSON value. */
    double                  double_val;         /**< Double JSON value. */
    jfes_string_t           string_val;         /**< String JSON value. */

    jfes_array_t            *array_val;         /**< Array JSON value. */
    jfes_object_t           *object_val;        /**< Object JSON value. */
} jfes_value_data_t;

/** JSON value structure. */
struct jfes_value {
    jfes_value_type_t       type;               /**< JSON value type. */
    jfes_value_data_t       data;               /**< Value data. */
};

/** 
    JFES status analizer function.

    \param[in]      status              Status variable.

    \return Zero, if status not equals jfes_success. Otherwise anything else.
*/
int jfes_status_is_good(jfes_status_t status);

/**
JFES status analizer function.

\param[in]      status              Status variable.

\return Zero, if status equals jfes_success. Otherwise anything else.
*/
int jfes_status_is_bad(jfes_status_t status);

/**
    JFES parser initialization.

    \param[out]     parser              Pointer to the jfes_parser_t object.
    \param[in]      config              JFES configuration.

    \return         jfes_success if everything is OK.
*/
jfes_status_t jfes_init_parser(jfes_parser_t *parser, jfes_config_t *config);

/**
    Resets all parser fields, except memory allocation functions.

    \param[out]     parser              Pointer to the jfes_parser_t object.

    \return         jfes_success if everything is OK.
*/
jfes_status_t jfes_reset_parser(jfes_parser_t *parser);

/**
    Run JSON parser. It parses a JSON data string into and 
    array of tokens, each describing a single JSON object.

    \param[in]      parser              Pointer to the jfes_parser_t object.
    \param[in]      json                JSON data string.
    \param[in]      length              JSON data length.
    \param[out]     tokens              Tokens array to fill.
    \param[in, out] max_tokens_count    Maximal count of tokens in tokens array.
                                        Will contain tokens count.

    \return         jfes_success if everything is OK.
*/
jfes_status_t jfes_parse_tokens(jfes_parser_t *parser, const char *json, 
    jfes_size_t length, jfes_token_t *tokens, jfes_size_t *max_tokens_count);

/**
    Run JSON parser and fills jfes_value_t object.

    \param[in]      config              JFES configuration.
    \param[in]      json                JSON data string.
    \param[in]      length              JSON data length.
    \param[out]     value               Output value;

    \return         jfes_success if everything is OK.
*/
jfes_status_t jfes_parse_to_value(jfes_config_t *config, const char *json,
    jfes_size_t length, jfes_value_t *value);

/**
    Free all resources, captured by object.

    \param[in]      config              JFES configuration.
    \param[in,out]  value               Object to free.

    \return         jfes_success if everything is OK.
*/
jfes_status_t jfes_free_value(jfes_config_t *config, jfes_value_t *value);

#endif