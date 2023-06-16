#ifndef HSE_HOMEWORKOPERATINGSYSTEMS_HW4_OPTIONALADDR_H_
#define HSE_HOMEWORKOPERATINGSYSTEMS_HW4_OPTIONALADDR_H_

#include <arpa/inet.h>
#include <stdbool.h>

typedef struct OptionalAddrStruct {
	struct sockaddr_in addr;
	bool has_value;
} OptionalAddr;

void set(OptionalAddr* optional_addr, struct sockaddr_in value) {
	optional_addr->has_value = true;
	optional_addr->addr = value;
}

#endif //HSE_HOMEWORKOPERATINGSYSTEMS_HW4_OPTIONALADDR_H_
