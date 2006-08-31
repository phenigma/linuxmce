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
#include <string>
#include <vector>

typedef std::vector<std::string> svector;
typedef std::vector<int> ivector;

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

struct stream_info {
    std::string name;
    svector links;
    int type;
};

typedef std::vector<stream_info> stream_info_vec;

struct card_info {
    struct v4l2_capability cap;
    std::string comment;
    int cardnr;
    int hw_flags;
    stream_info_vec streams;
};

typedef std::vector<card_info> card_info_vec;
card_info_vec cards;

struct card_info *find_card(const struct v4l2_capability *cap,
        int cardnr, int hw_flags, const std::string &comment)
{
    for (unsigned i = 0; i < cards.size(); i++) {
        if (!strcmp((char*)cap->bus_info, (char*)cards[i].cap.bus_info))
            return &cards[i];
    }
    card_info card;
    card.cap = *cap;
    card.cardnr = cardnr;
    card.hw_flags = hw_flags;
    card.comment = comment;
    cards.push_back(card);
    return &cards[cards.size() - 1];
}

void add_stream(struct card_info *card, int streamtype, int is_link, const std::string &device)
{
    stream_info *s = NULL;

    for (unsigned i = 0; i < card->streams.size(); i++) {
        if (card->streams[i].type == streamtype) {
            s = &card->streams[i];
            break;
        }
    }
    if (s == NULL) {
        stream_info info;
        info.type = streamtype;
        card->streams.push_back(info);
        s = &card->streams[card->streams.size() - 1];
    }
    if (!is_link) {
        s->name = device;
        return;
    }
    s->links.push_back(device);
}

int main(int argc, char **argv)
{
    char buf[100];
    svector devices;
    ivector devnums;
    ivector dev_is_link;
    int i;

    for (i = MINDEV; i < MAXDEV; i++) {
        char num[10] = "";
        int j;

        if (i >= 0) sprintf(num, "%d", i);

        for (j = 0; devnames[j]; j++) {
            struct stat st;

            sprintf(buf, "/dev/%s%s", devnames[j], num);
            if (lstat(buf, &st) == 0) {
                devnums.push_back(i);
                dev_is_link.push_back(S_ISLNK(st.st_mode));
                devices.push_back(buf);
            }
            sprintf(buf, "/dev/v4l/%s%s", devnames[j], num);
            if (lstat(buf, &st) == 0) {
                devnums.push_back(i);
                dev_is_link.push_back(S_ISLNK(st.st_mode));
                devices.push_back(buf);
            }
        }
    }

    for (unsigned i = 0; i < devices.size(); i++) {
        struct card_info *card;
        struct ivtv_driver_info info;
        struct ivtv_stream_info stream_info;
        struct v4l2_capability cap;
        int fd, res;
        std::string comment;
        int streamtype = -1;
        int cardnr = -1;
        int hw_flags = 0;

        fd = open(devices[i].c_str(), O_RDONLY);
        if (fd == -1)
            continue;
        res = ioctl(fd, VIDIOC_QUERYCAP, &cap);
        if (res < 0) {
            struct video_capability vcap;

            res = ioctl(fd, VIDIOCGCAP, &vcap);
            if (res < 0) {
                printf("no v4l device: %s\n", devices[i].c_str());
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
            comment = info.comment;
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
            else if (strstr(devices[i].c_str(), "vbi")) {
                if (devnums[i] < 8) streamtype = IVTV_ENC_STREAM_TYPE_VBI;
                else if (devnums[i] < 16) streamtype = IVTV_DEC_STREAM_TYPE_VBI;
                else streamtype = IVTV_DEC_STREAM_TYPE_VOUT;
            }
            else if (strstr(devices[i].c_str(), "radio")) {
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
            if (strstr(devices[i].c_str(), "vbi")) streamtype = IVTV_ENC_STREAM_TYPE_VBI;
            else if (strstr(devices[i].c_str(), "radio")) streamtype = IVTV_ENC_STREAM_TYPE_RAD;
            else streamtype = IVTV_ENC_STREAM_TYPE_YUV;
        }

        card = find_card(&cap, cardnr, hw_flags, comment);
        add_stream(card, streamtype, dev_is_link[i], devices[i]);
        close(fd);
    }

    for (unsigned i = 0; i < cards.size(); i++) {
        struct card_info *c = &cards[i];
        int v = c->cap.version;
        unsigned st;

        if (i) printf("\n");
        printf("driver:   %s", c->cap.driver);
        if (v) printf("-%d.%d.%d%s%s", v >> 16, (v >> 8) & 0xff, v & 0xff, 
                c->comment.length() ? " " : "", c->comment.c_str());
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
        for (st = 0; st < c->streams.size(); st++) {
            struct stream_info *s = &c->streams[st];
            unsigned l;

            printf("%20s: %s", s->name.c_str(), streams[s->type]);
            for (l = 0; l < s->links.size(); l++) {
                if (l == 0) printf(" (links: ");
                else  printf(", ");
                printf("%s", s->links[l].c_str());
            }
            if (s->links.size()) printf(")");
            printf("\n");
        }
    }
    return 0;
}
