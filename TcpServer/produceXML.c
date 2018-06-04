#include "tcp.h"

int main(){
    xmlKeepBlanksDefault(0);
    xmlIndentTreeOutput = 1;
    xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"Account");
    xmlDocSetRootElement(doc,root_node);
    int n,i;
    char num[MAXLINE],user[MAXLINE],pass[MAXLINE];
    printf("how many user do you want create?\n");
    scanf("%d",&n);
    for(i = 0;i < n;i++)
    {

        printf("please input the %dth's user:",i);
        scanf("%s",user);

        printf("please input the %dth's password:",i);
        scanf("%s",pass);
        sprintf(num, "user-%d", i);
        xmlNodePtr node = xmlNewNode(NULL,BAD_CAST(num));   //增加用户子节点
        xmlAddChild(root_node,node);
        //为用户子节点加属性
        xmlNewTextChild(node,NULL,BAD_CAST"user",BAD_CAST(user));
        xmlNewTextChild(node,NULL,BAD_CAST"pass",BAD_CAST(pass));
        xmlNewTextChild(node,NULL,BAD_CAST"port",BAD_CAST"8000");

    }
    printf("begin s\n");
    int ret = xmlSaveFormatFile("account.xml",doc,1);
    if(ret != -1){
             printf("A xml doc is created;");
        }
    xmlFreeDoc(doc);
    return 0;
   }
