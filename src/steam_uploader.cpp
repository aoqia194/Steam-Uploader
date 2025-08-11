#include "Uploader.h"

// Tries to implement this:
// https://partner.steamgames.com/doc/features/workshop/implementation#uploading_a_workshop_item


int main() {
    PublishedFileId_t workshopID = 3545758025;
    Uploader uploader(workshopID);
    uploader.UpdateItem();
    return 0;
}
