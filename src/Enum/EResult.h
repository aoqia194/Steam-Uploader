#pragma once
#include <array>
#include <cstddef>
#include "steam/steam_api.h"


// array for EResult
// https://partner.steamgames.com/doc/api/steam_api#EResult

// max size of the array
constexpr size_t EResultCount = k_EResultTooManyPending + 1;

// type of elements in the array
struct EResultInfo {
    const char* name; // directly link to the enum name
    const char* description; // it's description
};

// array
constexpr std::array<EResultInfo, EResultCount> EResultTable {{
    { "EResult Invalid", "Invalid EResult, you're not supposed to get this." },	// 0
    { "k_EResultOK", "Success." }, // 1
    { "k_EResultFail", "Generic failure." }, // 2
    { "k_EResultNoConnection", "Your Steam client doesn't have a connection to the back-end." }, // 3
    { "k_EResultInvalidPassword", "Password/ticket is invalid." }, // 5
    { "k_EResultLoggedInElsewhere", "The user is logged in elsewhere." }, // 6
    { "k_EResultInvalidProtocolVer", "Protocol version is incorrect." }, // 7
    { "k_EResultInvalidParam", "A parameter is incorrect." }, // 8
    { "k_EResultFileNotFound", "File was not found." }, // 9
    { "k_EResultBusy", "Called method is busy - action not taken." }, // 10
    { "k_EResultInvalidState", "Called object was in an invalid state." }, // 11
    { "k_EResultInvalidName", "The name was invalid." }, // 12
    { "k_EResultInvalidEmail", "The email was invalid." }, // 13
    { "k_EResultDuplicateName", "The name is not unique." }, // 14
    { "k_EResultAccessDenied", "Access is denied." }, // 15
    { "k_EResultTimeout", "Operation timed out." }, // 16
    { "k_EResultBanned", "The user is VAC2 banned." }, // 17
    { "k_EResultAccountNotFound", "Account not found." }, // 18
    { "k_EResultInvalidSteamID", "The Steam ID was invalid." }, // 19
    { "k_EResultServiceUnavailable", "The requested service is currently unavailable." }, // 20
    { "k_EResultNotLoggedOn", "The user is not logged on." }, // 21
    { "k_EResultPending", "Request is pending, it may be in process or waiting on third party." }, // 22
    { "k_EResultEncryptionFailure", "Encryption or Decryption failed." }, // 23
    { "k_EResultInsufficientPrivilege", "Insufficient privilege." }, // 24
    { "k_EResultLimitExceeded", "Too much of a good thing." }, // 25
    { "k_EResultRevoked", "Access has been revoked (used for revoked guest passes.)" }, // 26
    { "k_EResultExpired", "License/Guest pass the user is trying to access is expired." }, // 27
    { "k_EResultAlreadyRedeemed", "Guest pass has already been redeemed by account, cannot be used again." }, // 28
    { "k_EResultDuplicateRequest", "The request is a duplicate and the action has already occurred in the past, ignored this time." }, // 29
    { "k_EResultAlreadyOwned", "All the games in this guest pass redemption request are already owned by the user." }, // 30
    { "k_EResultIPNotFound", "IP address not found." }, // 31
    { "k_EResultPersistFailed", "Failed to write change to the data store." }, // 32
    { "k_EResultLockingFailed", "Failed to acquire access lock for this operation." }, // 33
    { "k_EResultLogonSessionReplaced", "The logon session has been replaced." }, // 34
    { "k_EResultConnectFailed", "Failed to connect." }, // 35
    { "k_EResultHandshakeFailed", "The authentication handshake has failed." }, // 36
    { "k_EResultIOFailure", "There has been a generic IO failure." }, // 37
    { "k_EResultRemoteDisconnect", "The remote server has disconnected." }, // 38
    { "k_EResultShoppingCartNotFound", "Failed to find the shopping cart requested." }, // 39
    { "k_EResultBlocked", "A user blocked the action." }, // 40
    { "k_EResultIgnored", "The target is ignoring sender." }, // 41
    { "k_EResultNoMatch", "Nothing matching the request found." }, // 42
    { "k_EResultAccountDisabled", "The account is disabled." }, // 43
    { "k_EResultServiceReadOnly", "This service is not accepting content changes right now." }, // 44
    { "k_EResultAccountNotFeatured", "Account doesn't have value, so this feature isn't available." }, // 45
    { "k_EResultAdministratorOK", "Allowed to take this action, but only because requester is admin." }, // 46
    { "k_EResultContentVersion", "A Version mismatch in content transmitted within the Steam protocol." }, // 47
    { "k_EResultTryAnotherCM", "The current CM can't service the user making a request, user should try another." }, // 48
    { "k_EResultPasswordRequiredToKickSession", "You are already logged in elsewhere, this cached credential login has failed." }, // 49
    { "k_EResultAlreadyLoggedInElsewhere", "The user is logged in elsewhere. (Use k_EResultLoggedInElsewhere instead!)" }, // 50
    { "k_EResultSuspended", "Long running operation has suspended/paused. (eg. content download.)" }, // 51
    { "k_EResultCancelled", "Operation has been canceled, typically by user. (eg. a content download.)" }, // 52
    { "k_EResultDataCorruption", "Operation canceled because data is ill formed or unrecoverable." }, // 53
    { "k_EResultDiskFull", "Operation canceled - not enough disk space." }, // 54
    { "k_EResultRemoteCallFailed", "The remote or IPC call has failed." }, // 55
    { "k_EResultPasswordUnset", "Password could not be verified as it's unset server side." }, // 56
    { "k_EResultExternalAccountUnlinked", "External account (PSN, Facebook...) is not linked to a Steam account." }, // 57
    { "k_EResultPSNTicketInvalid", "PSN ticket was invalid." }, // 58
    { "k_EResultExternalAccountAlreadyLinked", "External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first." }, // 59
    { "k_EResultRemoteFileConflict", "The sync cannot resume due to a conflict between the local and remote files." }, // 60
    { "k_EResultIllegalPassword", "The requested new password is not allowed." }, // 61
    { "k_EResultSameAsPreviousValue", "New value is the same as the old one. This is used for secret question and answer." }, // 62
    { "k_EResultAccountLogonDenied", "Account login denied due to 2nd factor authentication failure." }, // 63
    { "k_EResultCannotUseOldPassword", "The requested new password is not legal." }, // 64
    { "k_EResultInvalidLoginAuthCode", "Account login denied due to auth code invalid." }, // 65
    { "k_EResultAccountLogonDeniedNoMail", "Account login denied due to 2nd factor auth failure - and no mail has been sent." }, // 66
    { "k_EResultHardwareNotCapableOfIPT", "The users hardware does not support Intel's Identity Protection Technology (IPT)." }, // 67
    { "k_EResultIPTInitError", "Intel's Identity Protection Technology (IPT) has failed to initialize." }, // 68
    { "k_EResultParentalControlRestricted", "Operation failed due to parental control restrictions for current user." }, // 69
    { "k_EResultFacebookQueryError", "Facebook query returned an error." }, // 70
    { "k_EResultExpiredLoginAuthCode", "Account login denied due to an expired auth code." }, // 71
    { "k_EResultIPLoginRestrictionFailed", "The login failed due to an IP restriction." }, // 72
    { "k_EResultAccountLockedDown", "The current users account is currently locked for use. This is likely due to a hijacking and pending ownership verification." }, // 73
    { "k_EResultAccountLogonDeniedVerifiedEmailRequired", "The logon failed because the accounts email is not verified." }, // 74
    { "k_EResultNoMatchingURL", "There is no URL matching the provided values." }, // 75
    { "k_EResultBadResponse", "Bad Response due to a Parse failure, missing field, etc." }, // 76
    { "k_EResultRequirePasswordReEntry", "The user cannot complete the action until they re-enter their password." }, // 77
    { "k_EResultValueOutOfRange", "The value entered is outside the acceptable range." }, // 78
    { "k_EResultUnexpectedError", "Something happened that we didn't expect to ever happen." }, // 79
    { "k_EResultDisabled", "The requested service has been configured to be unavailable." }, // 80
    { "k_EResultInvalidCEGSubmission", "The files submitted to the CEG server are not valid." }, // 81
    { "k_EResultRestrictedDevice", "The device being used is not allowed to perform this action." }, // 82
    { "k_EResultRegionLocked", "The action could not be complete because it is region restricted." }, // 83
    { "k_EResultRateLimitExceeded", "Temporary rate limit exceeded, try again later, different from k_EResultLimitExceeded which may be permanent." }, // 84
    { "k_EResultAccountLoginDeniedNeedTwoFactor", "Need two-factor code to login." }, // 85
    { "k_EResultItemDeleted", "The thing we're trying to access has been deleted." }, // 86
    { "k_EResultAccountLoginDeniedThrottle", "Login attempt failed, try to throttle response to possible attacker." }, // 87
    { "k_EResultTwoFactorCodeMismatch", "Two factor authentication (Steam Guard) code is incorrect." }, // 88
    { "k_EResultTwoFactorActivationCodeMismatch", "The activation code for two-factor authentication (Steam Guard) didn't match." }, // 89
    { "k_EResultAccountAssociatedToMultiplePartners", "The current account has been associated with multiple partners." }, // 90
    { "k_EResultNotModified", "The data has not been modified." }, // 91
    { "k_EResultNoMobileDevice", "The account does not have a mobile device associated with it." }, // 92
    { "k_EResultTimeNotSynced", "The time presented is out of range or tolerance." }, // 93
    { "k_EResultSmsCodeFailed", "SMS code failure - no match, none pending, etc." }, // 94
    { "k_EResultAccountLimitExceeded", "Too many accounts access this resource." }, // 95
    { "k_EResultAccountActivityLimitExceeded", "Too many changes to this account." }, // 96
    { "k_EResultPhoneActivityLimitExceeded", "Too many changes to this phone." }, // 97
    { "k_EResultRefundToWallet", "Cannot refund to payment method, must use wallet." }, // 98
    { "k_EResultEmailSendFailure", "Cannot send an email." }, // 99
    { "k_EResultNotSettled", "Can't perform operation until payment has settled." }, // 100
    { "k_EResultNeedCaptcha", "The user needs to provide a valid captcha." }, // 101
    { "k_EResultGSLTDenied", "A game server login token owned by this token's owner has been banned." }, // 102
    { "k_EResultGSOwnerDenied", "Game server owner is denied for some other reason such as account locked, community ban, vac ban, missing phone, etc." }, // 103
    { "k_EResultInvalidItemType", "The type of thing we were requested to act on is invalid." }, // 104
    { "k_EResultIPBanned", "The IP address has been banned from taking this action." }, // 105
    { "k_EResultGSLTExpired", "This Game Server Login Token (GSLT) has expired from disuse; it can be reset for use." }, // 106
    { "k_EResultInsufficientFunds", "user doesn't have enough wallet funds to complete the action" }, // 107
    { "k_EResultTooManyPending", "There are too many of this thing pending already" }, // 108
}};

constexpr const char* GetEResultName(EResult eResult) {
    return (eResult >= 0 && static_cast<size_t>(eResult) < EResultTable.size())
        ? EResultTable[static_cast<size_t>(eResult)].name
        : "k_EResultNone";
}

constexpr const char* GetEResultDescription(EResult eResult) {
    return (eResult >= 0 && static_cast<size_t>(eResult) < EResultTable.size())
        ? EResultTable[static_cast<size_t>(eResult)].description
        : "No description available.";
}
