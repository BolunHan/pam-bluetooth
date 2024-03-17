#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <security/_pam_types.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define SIZE_BUFF 2048
#define CONF_PATH "/etc/security/authorized_bluetooth.conf"

int parser(char ***addrs){
    // Parse the MAC Addresses
    if (access(CONF_PATH, F_OK) != 0)
        return -1;

    char buff[SIZE_BUFF] = {0};
    int fd = open(CONF_PATH, O_RDONLY);
    char c;
    int i = 0;
    // Store all the file in the buffer
    while (read(fd, &buff[i], sizeof(char))>0 && i<SIZE_BUFF)
        i++;

    // A Mac address is FF:FF:FF:FF:FF:FF\n so 18 characters long
    int nbmacaddrs = i/18+1;
    (*addrs) = (char**)malloc(nbmacaddrs*sizeof(char));
    for (i = 0; i < nbmacaddrs; i++){
        (*addrs)[i] = (char*)malloc(18*sizeof(char));
        for (int j = 0; j < 18; j++){
            (*addrs)[i][j] = buff[i*18+j];
        }
        (*addrs)[i][17] = '\0';
    }

    return nbmacaddrs;
}

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	printf("Acct mgmt\n");
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
    char **addrs;
    int nbmacaddrs = parser(&addrs);

    for (int i = 0; i < nbmacaddrs; i++){
        // Ensure the MAC Address is correct
        if (addrs[i][2] != ':' ||
            addrs[i][5] != ':' ||
            addrs[i][8] != ':' ||
            addrs[i][11] != ':' ||
            addrs[i][14] != ':'){continue;}

        char cmd[100] = "";

        // check if already connected
        snprintf(cmd, sizeof(cmd), "bluetoothctl -- info %s | grep -q 'Connected: yes'", addrs[i]);
        int x = system(cmd);
        if (x == 0){
            return PAM_SUCCESS;
        }

        // not connected yet, try to connect
        snprintf(cmd, sizeof(cmd), "timeout 5 bluetoothctl -- connect %s > /dev/null 2>&1", addrs[i]);
        system(cmd);

        // connected now?
        snprintf(cmd, sizeof(cmd), "bluetoothctl -- info %s | grep -q 'Connected: yes'", addrs[i]);
        int y = system(cmd);
        if (y == 0){
            // disconnect, as device was not connected before (to save battery)
            snprintf(cmd, sizeof(cmd), "timeout 5 bluetoothctl -- disconnect %s > /dev/null 2>&1", addrs[i]);
            system(cmd);
            return PAM_SUCCESS;
        }
    }
    return PAM_IGNORE;
}
