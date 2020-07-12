//////////////////////////////////////////////////////////////////////////
//  ehci.h  v01.10.00
//////////////////////////////////////////////////////////////////////////

#ifndef FYSOS__EHCI
#define FYSOS__EHCI

#pragma pack(1)


#define EHC_CAPS_CapLength      0x00
#define EHC_CAPS_Reserved       0x01
#define EHC_CAPS_IVersion       0x02
#define EHC_CAPS_HCSParams      0x04
#define EHC_CAPS_HCCParams      0x08
#define EHC_CAPS_HCSPPortRoute  0x0C

#define EHC_OPS_USBCommand       0x00
#define EHC_OPS_USBStatus        0x04
#define EHC_OPS_USBInterrupt     0x08
#define EHC_OPS_FrameIndex       0x0C
#define EHC_OPS_CtrlDSSegemnt    0x10
#define EHC_OPS_PeriodicListBase 0x14
#define EHC_OPS_AsyncListBase    0x18
#define EHC_OPS_ConfigFlag       0x40
#define EHC_OPS_PortStatus       0x44  // first port

#define EHCI_PORT_CCS            (1<<0)
#define EHCI_PORT_CSC            (1<<1)
#define EHCI_PORT_ENABLED        (1<<2)
#define EHCI_PORT_ENABLE_C       (1<<3)
#define EHCI_PORT_OVER_CUR_C     (1<<5)
#define EHCI_PORT_RESET          (1<<8)
#define EHCI_PORT_LINE_STATUS    (3<<10)
#define EHCI_PORT_PP             (1<<12)
#define EHCI_PORT_OWNER          (1<<13)

#define EHC_LEGACY_USBLEGSUP     0x00
#define EHC_LEGACY_USBLEGCTLSTS  0x04

#define EHC_LEGACY_TIMEOUT     10  // 10 milliseconds
#define EHC_LEGACY_BIOS_OWNED  (1<<16)
#define EHC_LEGACY_OS_OWNED    (1<<24)
#define EHC_LEGACY_OWNED_MASK  (EHC_LEGACY_BIOS_OWNED | EHC_LEGACY_OS_OWNED)

#define EHCI_PORT_WRITE_MASK     0x007FF1EE

#define EHCI_QUEUE_HEAD_PTR_MASK  0x1F

// HC uses the first 48 (68 if 64-bit) bytes, but each queue must be 32 byte aligned
#define EHCI_QUEUE_HEAD_SIZE  96  // 96 bytes

#define EHCI_QH_OFF_HORZ_PTR           0  // offset of item within queue head
#define EHCI_QH_OFF_ENDPT_CAPS         4
#define EHCI_QH_OFF_HUB_INFO           8
#define EHCI_QH_OFF_CUR_QTD_PTR       12
#define EHCI_QH_OFF_NEXT_QTD_PTR      16
#define EHCI_QH_OFF_ALT_NEXT_QTD_PTR  20
#define EHCI_QH_OFF_STATUS            24
#define EHCI_QH_OFF_BUFF0_PTR         28
#define EHCI_QH_OFF_BUFF1_PTR         32
#define EHCI_QH_OFF_BUFF2_PTR         36
#define EHCI_QH_OFF_BUFF3_PTR         40
#define EHCI_QH_OFF_BUFF4_PTR         44
#define EHCI_QH_OFF_BUFF0_HI          48
#define EHCI_QH_OFF_BUFF1_HI          52
#define EHCI_QH_OFF_BUFF2_HI          56
#define EHCI_QH_OFF_BUFF3_HI          60
#define EHCI_QH_OFF_BUFF4_HI          64
#define EHCI_QH_OFF_PREV_PTR          92  // we use this for our insert/remove queue stuff


#define QH_HS_T0         (0<<0)  // pointer is valid
#define QH_HS_T1         (1<<0)  // pointer is not valid

#define QH_HS_TYPE_ISO   (0<<1)  // ISO
#define QH_HS_TYPE_QH    (1<<1)  // Queue Head

#define QH_HS_EPS_FS     (0<<12) // Full speed endpoint
#define QH_HS_EPS_LS     (1<<12) // Low  speed endpoint
#define QH_HS_EPS_HS     (2<<12) // High speed endpoint


#define EHCI_TD_SIZE  64  // 64 bytes

#define EHCI_TD_OFF_NEXT_TD_PTR        0  // offset of item within td
#define EHCI_TD_OFF_ALT_NEXT_QTD_PTR   4
#define EHCI_TD_OFF_STATUS             8
#define EHCI_TD_OFF_BUFF0_PTR         12
#define EHCI_TD_OFF_BUFF1_PTR         16
#define EHCI_TD_OFF_BUFF2_PTR         20
#define EHCI_TD_OFF_BUFF3_PTR         24
#define EHCI_TD_OFF_BUFF4_PTR         28
#define EHCI_TD_OFF_BUFF0_HI          32
#define EHCI_TD_OFF_BUFF1_HI          36
#define EHCI_TD_OFF_BUFF2_HI          40
#define EHCI_TD_OFF_BUFF3_HI          44
#define EHCI_TD_OFF_BUFF4_HI          48

#define EHCI_TD_PID_OUT    0
#define EHCI_TD_PID_IN     1
#define EHCI_TD_PID_SETUP  2


// ISO's
#define EHCI_ISO_OFF_NEXT_TD_PTR        0  // offset of item within td
#define EHCI_ISO_OFF_STATUS_0           4
#define EHCI_ISO_OFF_STATUS_1           8
#define EHCI_ISO_OFF_STATUS_2          12
#define EHCI_ISO_OFF_STATUS_3          16
#define EHCI_ISO_OFF_STATUS_4          20
#define EHCI_ISO_OFF_STATUS_5          24
#define EHCI_ISO_OFF_STATUS_6          28
#define EHCI_ISO_OFF_STATUS_7          32
#define EHCI_ISO_OFF_BUFFER_0          36
#define EHCI_ISO_OFF_BUFFER_1          40
#define EHCI_ISO_OFF_BUFFER_2          44
#define EHCI_ISO_OFF_BUFFER_3          48
#define EHCI_ISO_OFF_BUFFER_4          52
#define EHCI_ISO_OFF_BUFFER_5          56
#define EHCI_ISO_OFF_BUFFER_6          60


#endif // FYSOS__EHCI
