#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h" //using void md5String(const char* input, char* output);
#include "md4.h" //using void md4String(const char* input, char* output);

#define MD4_HASH 0
#define MD5_HASH 1
#define True 1
#define False 0
typedef unsigned char HashType;
typedef unsigned char Byte;
typedef char Bool;

typedef struct msgWithHash
{
	unsigned int id;
	char* text;
	union
	{
		Byte md4[16];
		Byte md5[16];
	} hashDigest;
	HashType type;
}msgWithHash;

typedef struct msgListNode msgListNode;
struct msgListNode
{
	msgWithHash* data;
	msgListNode* next;
};

void print_hash(const unsigned char* p);
msgWithHash* createMessage(unsigned int id, const char* text, HashType type);
void printMessage(const msgWithHash* message);
msgListNode* addMessage(msgListNode* head, msgWithHash* data);
int findMsgByHashIterative(const msgListNode* head, Byte hash[16]);
int findMsgByHashRecursive(const msgListNode* head, Byte hash[16]);
msgListNode* deleteMessage(msgListNode* head, unsigned int id);
Bool verifyIntegrity(msgListNode* head, unsigned int id, Byte compareHash[16]);
msgListNode* reverseMsgList(msgListNode* head);
void freeMsgList(msgListNode* head);
void printMsgList(msgListNode* head);

void main()
{
	msgWithHash* message1 = createMessage(2, "hello world", MD4_HASH);
	msgWithHash* message2 = createMessage(1, "MY NAME IS SHAHAR", MD5_HASH);
	msgWithHash* message3 = createMessage(10, "good morning", MD4_HASH);
	msgWithHash* message4 = createMessage(5, "good night", MD5_HASH);
	printMessage(message1);
	printMessage(message2);
	printMessage(message3);
	printMessage(message4);
	msgListNode* listhead = NULL;
	listhead = addMessage(listhead, message1);
	printMsgList(listhead);
	listhead = addMessage(listhead, message2);
	printMsgList(listhead);
	listhead = addMessage(listhead, message3);
	printMsgList(listhead);
	listhead = addMessage(listhead, message4);
	printMsgList(listhead);
	int rc = findMsgByHashIterative(listhead, message1->hashDigest.md4);
	printf("findMsgByHashIterative:find message 1 md4string rc=%d\n", rc);
	rc = findMsgByHashIterative(listhead, message2->hashDigest.md5);
	printf("findMsgByHashIterative:find message 2 md5string rc=%d\n", rc);
	rc = findMsgByHashRecursive(listhead, message1->hashDigest.md4);
	printf("findMsgByHashRecursive:find message 1 md4string rc=%d\n", rc);
	rc = findMsgByHashRecursive(listhead, message2->hashDigest.md5);
	printf("findMsgByHashRecursive:find message 2 md5string rc=%d\n", rc);
	listhead = deleteMessage(listhead, 1);
	printMsgList(listhead);
	rc = findMsgByHashIterative(listhead, message1->hashDigest.md4);
	printf("findMsgByHashIterative: find message 1 md4string rc=%d\n", rc);
	rc = verifyIntegrity(listhead, 2, message2->hashDigest.md4);
	printf("verifyIntegrity: find message 2 md5string rc=%d\n", rc);
	listhead = reverseMsgList(listhead);
	printMsgList(listhead);
	freeMsgList(listhead);


}


/******************** Example 1 from PDF ********************************************/

/*const char* sentence = "this is some text that I want to hash.";
printf("text: \"%s\"\n", sentence);
unsigned char hash[16] = { 0 };
md5String(sentence, hash);
printf("md5: ");
print_hash(hash);

printf("md4: ");
md4String(sentence, hash);
print_hash(hash);*/

/******************** Example 2 from PDF ********************************************/

/*const msgWithHash* msg1 = createMessage(87651233, "", MD4_HASH);
const msgWithHash* msg2 = createMessage(3, "banana1", MD5_HASH);
const msgWithHash* msg3 = createMessage(275, "banana2!", MD4_HASH);
const msgWithHash* msg4 = createMessage(342234, "Hello World", MD4_HASH);

printMessage(msg1);
printMessage(msg2);
printMessage(msg3);
printMessage(msg4);*/



void print_hash(const unsigned char* p) {
	for (unsigned int i = 0; i < 16; ++i) {
		printf("%02x", p[i]);
	}
	printf("\n");
}

msgWithHash* createMessage(unsigned int id, const char* text, HashType type)
{

	if (type != MD4_HASH && type != MD5_HASH) //check type parmeter 
		return NULL;

	msgWithHash* newmessage = (msgWithHash*)calloc(1, sizeof(msgWithHash));
	if (newmessage == NULL)
	{
		return NULL;
	}
	newmessage->id = id;
	newmessage->text = text;
	newmessage->type = type;
	if (type == MD4_HASH)
	{
		md4String(text, newmessage->hashDigest.md4);
	}
	else
		md5String(text, newmessage->hashDigest.md5);
	return newmessage;

}
void printMessage(const msgWithHash* message)
{
	if (message == NULL)
		return NULL;
	printf("Message id: %d\n", message->id);
	printf("message: \"%s\"\n", message->text);
	if (message->type == MD4_HASH)
	{
		printf("Md4 hash: ");
		print_hash(message->hashDigest.md4);
	}
	else
	{
		printf("Md5 hash: ");
		print_hash(message->hashDigest.md5);
	}
}
msgListNode* addMessage(msgListNode* head, msgWithHash* data)
{
	if (data == NULL)
		return NULL;
	msgListNode* p = head;
	msgListNode* prev = head;
	msgListNode* element = (msgListNode*)calloc(1, sizeof(msgListNode));
	if (element == NULL)
		return NULL;
	element->data = data;
	element->next = NULL;
	if (head == NULL)
		return element;

	while (p != NULL)
	{
		if (p->data->id < data->id)
		{
			prev = p;
			p = p->next;
		}
		else
		{
			if (p == head)
			{
				element->next = head;
				return element;
			}
			prev->next = element;
			element->next = p;
			return head;
		}

	}
	prev->next = element;
	return head;
}
int findMsgByHashIterative(const msgListNode* head, Byte hash[16])
{
	msgListNode* p = head;
	int counter = 1;
	if (head == NULL)
		return -2;
	while (p != NULL)
	{
		if (p->data->type == MD4_HASH)
		{
			if (memcmp(p->data->hashDigest.md4, hash, sizeof(hash)) == 0)
				return counter;

		}
		else
		{
			if (memcmp(p->data->hashDigest.md5, hash, sizeof(hash)) == 0)
				return counter;

		}
		counter++;
		p = p->next;


	}
	return -1;

}
int findMsgByHashRecursive(const msgListNode* head, Byte hash[16])
{
	msgListNode* p = head;
	static int counter = 0;
	int tmp = 0;
	if (head = NULL)
		return -1;

	counter++;
	if (p->data->type = MD4_HASH)
	{
		if (memcmp(p->data->hashDigest.md4, hash, sizeof(hash)) == 0)
		{
			tmp = counter;
			counter = 0;
			return tmp;
		}
		return findMsgByHashRecursive(p->next, hash);


	}
	else
	{
		if (memcmp(p->data->hashDigest.md5, hash, sizeof(hash)) == 0)
		{
			tmp = counter;
			counter = 0;
			return tmp;
		}
		return findMsgByHashRecursive(p->next, hash);
	}
	return -1;

}

msgListNode* deleteMessage(msgListNode* head, unsigned int id)
{
	msgListNode* p = head;
	msgListNode* tmp = NULL;
	msgListNode* prev = NULL;
	if (head == NULL)
		return NULL;
	if (p->data->id == id)
	{
		tmp = p->next;
		free(p->data);
		free(p);
		return tmp;

	}
	else
	{
		prev = p;
		p = p->next;
		while (p != NULL)
		{
			if (p->data->id == id)
			{
				prev->next = p->next;

				free(p->data);
				free(p);
				return head;
			}
			prev = p;
			p = p->next;
		}
	}
	return head;
}

Bool verifyIntegrity(msgListNode* head, unsigned int id, Byte compareHash[16])
{
	msgListNode* p = head;
	if (head == NULL)
		return 0;
	while (p != NULL)
	{
		if (p->data->id == id)
		{
			if (p->data->type == MD4_HASH)
			{
				if (memcmp(p->data->hashDigest.md4, compareHash, sizeof(compareHash)) == 0)
					return 1;


			}
			else
			{
				if (memcmp(p->data->hashDigest.md5, compareHash, sizeof(compareHash)) == 0)
					return 1;

			}

		}
		p = p->next;


	}
	return -1;

}

msgListNode* reverseMsgList(msgListNode* head)
{
	msgListNode* p = head;
	msgListNode* next = NULL;
	msgListNode* prev = NULL;

	while (p != NULL)
	{
		next = p->next;
		p->next = prev;
		prev = p;
		p = next;
	}
	return prev;
}
void freeMsgList(msgListNode* head)
{
	msgListNode* p = head;
	msgListNode* tmp = NULL;
	while (p != NULL)
	{
		tmp = p->next;
		free(p->data);
		free(p);
		p = tmp;
	}
}
void printMsgList(msgListNode* head)
{
	msgListNode* p = head;
	int counter = 0;

	while (p != NULL)
	{
		counter++;
		printf("element %d id: %d text: %s \n", counter, p->data->id, p->data->text);
		p = p->next;
	}
}