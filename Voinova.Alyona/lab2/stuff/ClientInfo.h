//
// Created by pikabol88 on 11/27/21.
//

#ifndef LAB2_CLIENTINFO_H
#define LAB2_CLIENTINFO_H


class ClientInfo {
public:
    explicit ClientInfo(int pid);

    int getPid() const;
    bool isConnected() const;
    void setConnected(bool _isConnected);

private:
    int _pid;
    bool _isConnected;

};


#endif //LAB2_CLIENTINFO_H
