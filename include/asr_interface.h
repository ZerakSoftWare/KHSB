#ifndef __ASR_INTERFACE_H__
#define __ASR_INTERFACE_H__

#define SAMPLE_RATE_8000    8000
#define SAMPLE_RATE_16000   16000

#define FS_ASR_DATA_LEN     320

#define FS_ASR_STRING_LEN   255

typedef enum fs_asr_message_type_e
{
    FS_ASR_REQUEST_NULL = 100,

    FS_ASR_START_ASR_REQUEST,
    FS_ASR_START_ASR_RESPONSE,
    FS_ASR_STOP_ASR_REQUEST,
    FS_ASR_STOP_ASR_RESPONSE,
    
    FS_ASR_ASR_DATA,
    
}fs_asr_message_type;

typedef struct fs_data_t
{
    int sample_rate;
    
    int data_len;
    char data[FS_ASR_DATA_LEN];
    
}fs_data;

#if 0
typedef union fs_request_union
{
    fs_start_asr_request    stStartAsrRequest;
    fs_data                 stData;
    
}fs_request_un;
#endif

typedef struct fs_asr_message_t
{
    int msg_type;
    int msg_len;
    
    char uuid[FS_ASR_STRING_LEN];
    
    int seq;
    int rate;
    
    int data_len;
    char data[FS_ASR_DATA_LEN];
    
}fs_asr_message;


typedef enum asr_fs_msg_type_e
{
    ASR_FS_INITIAL,
        
    ASR_FS_STOP_ASR,
        
    
}asr_fs_msg_type;

typedef enum stop_asr_reason_e
{
    REASON_NONE,
        
    REASON_NOT_AVAILABLE,   // 无法接通
    REASON_POWER_OFF,       // 已关机
    REASON_BUSY,            // 用户忙
    REASON_NOT_IN_SERVICE,  // 不在服务区    

}stop_asr_reason;

typedef struct asr_fs_header_t
{
    int msg_type;
    int msg_len;
}asr_fs_header;


typedef struct asr_fs_stop_asr_request_t
{
    char uuid[FS_ASR_STRING_LEN];
    
    // stop_asr_reason
    char reason;    
    
}asr_fs_stop_asr_request;

typedef struct asr_fs_message_t
{
    asr_fs_header header;
    
    asr_fs_stop_asr_request stStopAsrRequest;
    
}asr_fs_message;

#endif

