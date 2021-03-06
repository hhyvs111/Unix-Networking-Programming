#include "tcp.h"

/*
 * 函数功能：
 * 将用户名和密码传入，然后解析XML，查看是否有匹配值，返回匹配结果
 */
int main()
{
    char user[30],pass[30];
    scanf("%s %s",user,pass);
    xmlDocPtr doc;
    xmlNodePtr curNode;
    xmlChar* userKey,*passKey;
    xmlKeepBlanksDefault(0);//必须加上，防止程序把元素前后的空白文本符号当作一个node
    doc = xmlReadFile ("account.xml", "UTF-8", XML_PARSE_RECOVER);//libxml只能解析UTF-8格式数据

    if(doc == NULL)
    {
        printf("open xml failed!\n");
        return -1;
    }

    curNode = xmlDocGetRootElement(doc);

    if(curNode == NULL)
    {
        printf("empty document.\n");
        return -1;
    }
    if(xmlStrcmp(curNode->name,BAD_CAST"Account"))
    {
        printf("root node is not Account\n");
        return -1;
    }

    xmlNodePtr numNode = curNode->xmlChildrenNode;   //curNode为account根结点，它的子节点是num结点（用户数量结点）
    /*printf("the name:%s ,the curNode : %s \n",numNode->name,curNode->name);*/
    /*printf("content: %s \n",xmlNodeGetContent(numNode));*/
    while(NULL != numNode)   //依次遍历用户
    {
        curNode = numNode->xmlChildrenNode;   //此时curNode指向了第Num用户结点的用户名
        printf("curNode:%s\n",curNode->name);
        if(!xmlStrcmp(curNode->name,BAD_CAST"user"))   //需要判断是否当前结点是user结点i
        {
            userKey = xmlNodeGetContent(curNode); //获取xml里的用户名
        }
        else
        {
            printf("is not correct xml\n");
            return -1;
        }
        curNode = curNode->next;   //指向password


        if((!xmlStrcmp(curNode->name,BAD_CAST"pass")))
        {
            passKey = xmlNodeGetContent(curNode);
        }
        else
        {
            printf("is not correct xml\n");
            return -1;
        }
        if(strcmp(user,userKey) == 0 && strcmp(pass,passKey) == 0)
        {
            printf("Login success!\n");
            xmlFree(userKey);
            xmlFree(passKey);
            return 1;
        }
        numNode = numNode->next;  //继续遍历下一个
            xmlFree(userKey);
            xmlFree(passKey);
        printf("test\n");
    }
    printf("not match user!\n");
    return 0;
}
