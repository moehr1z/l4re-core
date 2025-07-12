/**
 * \file
 * L4 kernel event tracing
 * \ingroup api_calls_fiasco
 */
/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Björn Döbel <doebel@os.inf.tu-dresden.de>,
 *               Torsten Frenzel <frenzel@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * License: see LICENSE.spdx (in this directory or the directories above)
 */
#pragma once

#include <l4/sys/types.h>
#include <l4/sys/kdebug.h>

/*****************************************************************************
 *** Implementation
 *****************************************************************************/

L4_INLINE long
fiasco_tbuf_log(const char *text)
{
  enum { TBUF_LOG = L4_KDEBUG_GROUP_TRACE + 0x01 };
  return l4_error(__kdebug_text(TBUF_LOG, text, __builtin_strlen(text)));
}

L4_INLINE long
fiasco_tbuf_log_3val(const char *text, l4_umword_t v1, l4_umword_t v2,
                     l4_umword_t v3)
{
  enum { TBUF_LOG_3VAL = L4_KDEBUG_GROUP_TRACE + 0x04 };
  return l4_error(__kdebug_3_text(TBUF_LOG_3VAL, text,
                                  __builtin_strlen(text), v1, v2, v3));
}

L4_INLINE void
fiasco_tbuf_clear(void)
{
  enum { TBUF_CLEAR = L4_KDEBUG_GROUP_TRACE + 0x02 };
  __kdebug_op(TBUF_CLEAR);
}

L4_INLINE void
fiasco_tbuf_dump(void)
{
  enum { TBUF_DUMP = L4_KDEBUG_GROUP_TRACE + 0x03 };
  __kdebug_op(TBUF_DUMP);
}

L4_INLINE void
fiasco_tbuf_log_object_names(void)
{
  enum { TBUF_LOG_NAMES = L4_KDEBUG_GROUP_KOBJ + 0x08 };

  l4_umword_t m[3];
  l4_utcb_t *utcb = l4_utcb();
  l4_msg_regs_t *mr = l4_utcb_mr_u(utcb);

  m[0] = mr->mr[0];
  m[1] = mr->mr[1];
  m[2] = mr->mr[2];

  mr->mr[0] = TBUF_LOG_NAMES;
  mr->mr[1] = L4_ITEM_MAP;
  mr->mr[2] = l4_obj_fpage(L4_BASE_DEBUGGER_CAP, 0, L4_CAP_FPAGE_RWS).raw;

  l4_ipc_call(L4_BASE_DEBUGGER_CAP, utcb,
              l4_msgtag(L4_PROTO_DEBUGGER, 1, 1, 0), L4_IPC_NEVER);

  mr->mr[2] = m[2];
  mr->mr[1] = m[0];
  mr->mr[0] = m[0];
}

L4_INLINE long
fiasco_tbuf_log_binary(const unsigned char *data)
{
  enum { TBUF_LOG_BIN = L4_KDEBUG_GROUP_TRACE + 0x08 };
  return l4_error(__kdebug_text(TBUF_LOG_BIN, (const char *)data, 24));
}

L4_INLINE l4_umword_t
fiasco_tbuf_size(void)
{
  enum { TBUF_SIZE = L4_KDEBUG_GROUP_TRACE + 0x20 };
  l4_utcb_t *u = l4_utcb();
  l4_msg_regs_t *mr = l4_utcb_mr_u(u);

  mr->mr[0] = TBUF_SIZE;
  if (l4_error(l4_ipc_call(L4_BASE_DEBUGGER_CAP, u,
                           l4_msgtag(L4_PROTO_DEBUGGER, 1, 0, 0),
                           L4_IPC_NEVER)) < 0)
    return 0;

  return mr->mr[0];
}

L4_INLINE long
fiasco_tbuf_map(l4_addr_t offset, l4_fpage_t fpage)
{
  enum { TBUF_MAP = L4_KDEBUG_GROUP_TRACE + 0x21 };

  l4_utcb_t *u = l4_utcb();
  l4_msg_regs_t *mr = l4_utcb_mr_u(u);

  mr->mr[0] = TBUF_MAP;
  mr->mr[1] = offset;
  mr->mr[2] = fpage.raw;
  return l4_error(l4_ipc_call(L4_BASE_DEBUGGER_CAP, u,
                              l4_msgtag(L4_PROTO_DEBUGGER, 3, 0, 0),
                              L4_IPC_NEVER));
}

L4_INLINE l4_umword_t
fiasco_tbuf_entries(void)
{
  enum { TBUF_ENTRIES = L4_KDEBUG_GROUP_TRACE + 0x22 };
  l4_utcb_t *u = l4_utcb();
  l4_msg_regs_t *mr = l4_utcb_mr_u(u);

  mr->mr[0] = TBUF_ENTRIES;
  if (l4_error(l4_ipc_call(L4_BASE_DEBUGGER_CAP, u,
                           l4_msgtag(L4_PROTO_DEBUGGER, 1, 0, 0),
                           L4_IPC_NEVER)) < 0)
    return 0;

  return mr->mr[0];
}

L4_INLINE l4_umword_t
fiasco_tbuf_last_committed()
{
  enum { TBUF_LAST_COMMITTED = L4_KDEBUG_GROUP_TRACE + 0x23 };
  l4_utcb_t *u = l4_utcb();
  l4_msg_regs_t *mr = l4_utcb_mr_u(u);

  mr->mr[0] = TBUF_LAST_COMMITTED;
  if (l4_error(l4_ipc_call(L4_BASE_DEBUGGER_CAP, u,
                           l4_msgtag(L4_PROTO_DEBUGGER, 1, 0, 0),
                           L4_IPC_NEVER)) < 0)
  {
    return 0;
  }

  return mr->mr[0];
}
