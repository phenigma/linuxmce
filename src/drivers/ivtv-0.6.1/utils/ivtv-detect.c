#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <inttypes.h>
#define __user
#include <linux/videodev.h>
#define IVTV_INTERNAL
#include <ivtv.h>

static char *devnames[] = {
        "video",
        "radio",
        "vbi",
        NULL
};

static char *streams[] = {
        "MPG encoding",
        "YUV encoding",
        "VBI encoding",
        "PCM encoding",
        "Radio       ",
        "MPG decoding",
        "VBI from MPG",
        "VBI decoding",
        "YUV decoding",
        "OSD (should not be seen)"
};

// device -1 is a special code for the device names without a number
#define MINDEV -1
#define MAXDEV 100
#define MAXCARD 15

struct stream_info {
        char name[100];
        char links[4][100];
        int type;
};

struct card_info {
        struct v4l2_capability cap;
        char comment[100];
        int cardnr;
        int hw_flags;
        int maxstream;
        struct stream_info streams[10];
} cards[MAXCARD + 1];

int maxcard = 0;


struct card_info *find_card(const struct v4l2_capability *cap,
                            int cardnr, int hw_flags, const char *comment)
{
        int i;

        for (i = 0; i < maxcard; i++) {
                if (!strcmp((char*)cap->bus_info, (char*)cards[i].cap.bus_info))
                        return &cards[i];
        }
        cards[maxcard].cap = *cap;
        cards[maxcard].cardnr = cardnr;
        cards[maxcard].hw_flags = hw_flags;
        cards[maxcard].maxstream = 0;
        strcpy(cards[maxcard].comment, comment);
        maxcard++;
        return &cards[maxcard - 1];
}

void add_stream(struct card_info *card, int streamtype, int is_link, const char *device)
{
        struct stream_info *s = NULL;
        int i;

        for (i = 0; i < card->maxstream; i++) {
                if (card->streams[i].type == streamtype) {
                        s = &card->streams[i];
                        break;
                }
        }
        if (s == NULL) {
                card->streams[card->maxstream].type = streamtype;
                card->maxstream++;
                s = &card->streams[card->maxstream - 1];
        }
        if (!is_link) {
                strcpy(s->name, device);
                return;
        }
        i = 0;
        while (s->links[i][0]) i++;
        strcpy(s->links[i], device);
}

int main(int argc, char **argv)
{
        char buf[100];
        char *devices[6 * (MAXDEV + 1 - MINDEV)];
        int devnums[6 * (MAXDEV + 1 - MINDEV)];
        int dev_is_link[6 * (MAXDEV + 1 - MINDEV)];
        int maxdev = 0;
        int i;

        for (i = MINDEV; i < MAXDEV; i++) {
                char num[10] = "";
                int j;

                if (i >= 0) sprintf(num, "%d", i);

                for (j = 0; devnames[j]; j++) {
                        struct stat st;

                        sprintf(buf, "/dev/%s%s", devnames[j], num);
                        if (lstat(buf, &st) == 0) {
                                devnums[maxdev] = i;
                                dev_is_link[maxdev] = S_ISLNK(st.st_mode);
                                devices[maxdev++] = strdup(buf);
                        }
                        sprintf(buf, "/dev/v4l/%s%s", devnames[j], num);
                        if (lstat(buf, &st) == 0) {
                                devnums[maxdev] = i;
                                dev_is_link[maxdev] = S_ISLNK(st.st_mode);
                                devices[maxdev++] = strdup(buf);
                        }
                }
        }

        for (i = 0; i < maxdev; i++) {
                struct card_info *card;
                struct ivtv_driver_info info;
                struct ivtv_stream_info stream_info;
                struct v4l2_capability cap;
                int fd, res;
                char comment[sizeof(info.comment)] = "";
                int streamtype = -1;
                int cardnr = -1;
                int hw_flags = 0;

                fd = open(devices[i], O_RDONLY);
		if (fd == -1)
			continue;
                res = ioctl(fd, VIDIOC_QUERYCAP, &cap);
                if (res < 0) {
                        struct video_capability vcap;

                        res = ioctl(fd, VIDIOCGCAP, &vcap);
                        if (res < 0) {
                                printf("no v4l device: %s\n", devices[i]);
                                close(fd);
                                continue;
                        }
                        strcpy((char*)cap.card, vcap.name);
                        strcpy((char*)cap.driver, "v4l");
                        strcpy((char*)cap.bus_info, "");
                        cap.version = 0;
                }

                memset(&info, 0, sizeof(info));
                info.size = sizeof(info);
                res = ioctl(fd, IVTV_IOC_G_DRIVER_INFO, &info);
                if (res < 0) {
                        // Stupid bug in V1 driver info code,
                        // it should just fall back and only fill in
                        // the area it knows about.
                        info.size = IVTV_DRIVER_INFO_V1_SIZE;
                        res = ioctl(fd, IVTV_IOC_G_DRIVER_INFO, &info);
                }
                if (res == 0) {
                        strcpy(comment, info.comment);
                        if (info.size == IVTV_DRIVER_INFO_V1_SIZE) {
                                cardnr = atol((char*)cap.bus_info);
                        }
                        else {
                                cardnr = info.cardnr;
                                hw_flags = info.hw_flags;
                        }
                        stream_info.size = sizeof(stream_info);
                        res = ioctl(fd, IVTV_IOC_G_STREAM_INFO, &stream_info);
                        if (res == 0) {
                                streamtype = stream_info.type;
                        }
                        else if (strstr(devices[i], "vbi")) {
                                if (devnums[i] < 4) streamtype = IVTV_ENC_STREAM_TYPE_VBI;
                                else if (devnums[i] < 8) streamtype = IVTV_DEC_STREAM_TYPE_VBI;
                                else streamtype = IVTV_DEC_STREAM_TYPE_VOUT;
                        }
                        else if (strstr(devices[i], "radio")) {
                                streamtype = IVTV_ENC_STREAM_TYPE_RAD;
                        }
                        else {
                                if (devnums[i] < 16) streamtype = IVTV_ENC_STREAM_TYPE_MPG;
                                else if (devnums[i] < 24) streamtype = IVTV_ENC_STREAM_TYPE_PCM;
                                else if (devnums[i] < 32) streamtype = IVTV_DEC_STREAM_TYPE_MPG;
                                else if (devnums[i] < 48) streamtype = IVTV_ENC_STREAM_TYPE_YUV;
                                else streamtype = IVTV_DEC_STREAM_TYPE_YUV;
                        }
                }
                else {
                        if (strstr(devices[i], "vbi")) streamtype = IVTV_ENC_STREAM_TYPE_VBI;
                        else if (strstr(devices[i], "radio")) streamtype = IVTV_ENC_STREAM_TYPE_RAD;
                        else streamtype = IVTV_ENC_STREAM_TYPE_YUV;
                }

                card = find_card(&cap, cardnr, hw_flags, comment);
                add_stream(card, streamtype, dev_is_link[i], devices[i]);
                close(fd);
        }

        for (i = 0; i < maxcard; i++) {
                struct card_info *c = &cards[i];
                int v = c->cap.version;
                int st;

                if (i) printf("\n");
                printf("driver:   %s", c->cap.driver);
                if (v) printf("-%d.%d.%d%s%s", v >> 16, (v >> 8) & 0xff, v & 0xff, 
                                c->comment[0] ? " " : "", c->comment);
                printf("\ncard:     %s", c->cap.card);
                if (c->cap.bus_info[0]) {
                        printf(", bus info %s", c->cap.bus_info);
                }
                if (c->cardnr >= 0) {
                        printf(", ivtv card #%d", c->cardnr);
                }
                printf("\n");
                if (c->hw_flags) {
                        static const char *hw[] = {
                                " cx25840",
                                " saa7115",
                                " saa7127",
                                " msp34xx",
                                " tuner",
                                " wm8775",
                                " cs53132a",
                                " tveeprom",
                                " saa7114",
				" tvaudio",
				" upd64031a",
				" upd6408x",
				" saa717x",
				" wm8739",
				" gpio",
                                NULL
                        };
                        int i;

                        printf("hardware:");
                        for (i = 0; hw[i]; i++) {
                                if (c->hw_flags & (1 << i)) {
                                        printf(hw[i]);
                                }
                        }
                        printf("\n");
                }
                for (st = 0; st < c->maxstream; st++) {
                        struct stream_info *s = &c->streams[st];
                        int l;

                        printf("%20s: %s", s->name, streams[s->type]);
                        for (l = 0; l < 4; l++) {
                                if (s->links[l][0]) {
                                        if (l == 0) printf(" (links: ");
                                        else  printf(", ");
                                        printf("%s", s->links[l]);
                                }
                        }
                        if (s->links[0][0]) printf(")");
                        printf("\n");
                }
        }
        return 0;
}
