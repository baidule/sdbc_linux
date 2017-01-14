CREATE OR REPLACE FORCE VIEW "SYS"."ALL_CONSTRAINTS" ("OWNER", "CONSTRAINT_NAME", "CONSTRAINT_TYPE", "TABLE_NAME", "SEARCH_CONDITION", "R_OWNER", "R_CONSTRAINT_NAME", "DELETE_RULE", "STATUS", "DEFERRABLE", "DEFERRED", "VALIDATED", "GENERATED", "BAD", "RELY", "LAST_CHANGE", "INDEX_OWNER", "INDEX_NAME", "INVALID", "VIEW_RELATED") AS 
  select ou.name, oc.name,
       decode(c.type#, 1, 'C', 2, 'P', 3, 'U',
              4, 'R', 5, 'V', 6, 'O', 7,'C', '?'),
       o.name, c.condition, ru.name, rc.name,
       decode(c.type#, 4,
              decode(c.refact, 1, 'CASCADE', 2, 'SET NULL', 'NO ACTION'),
              NULL),
       decode(c.type#, 5, 'ENABLED',
              decode(c.enabled, NULL, 'DISABLED', 'ENABLED')),
       decode(bitand(c.defer, 1), 1, 'DEFERRABLE', 'NOT DEFERRABLE'),
       decode(bitand(c.defer, 2), 2, 'DEFERRED', 'IMMEDIATE'),
       decode(bitand(c.defer, 4), 4, 'VALIDATED', 'NOT VALIDATED'),
       decode(bitand(c.defer, 8), 8, 'GENERATED NAME', 'USER NAME'),
       decode(bitand(c.defer,16),16, 'BAD', null),
       decode(bitand(c.defer,32),32, 'RELY', null),
       c.mtime,
       decode(c.type#, 2, ui.name, 3, ui.name, null),
       decode(c.type#, 2, oi.name, 3, oi.name, null),
       decode(bitand(c.defer, 256), 256,
              decode(c.type#, 4,
                     case when (bitand(c.defer, 128) = 128
                                or o.status in (3, 5)
                                or ro.status in (3, 5)) then 'INVALID'
                          else null end,
                     case when (bitand(c.defer, 128) = 128
                                or o.status in (3, 5)) then 'INVALID'
                          else null end
                    ),
              null),
       decode(bitand(c.defer, 256), 256, 'DEPEND ON VIEW', null)
from sys.con$ oc, sys.con$ rc, sys.user$ ou, sys.user$ ru, sys.obj$ ro,
     sys.obj$ o, sys.cdef$ c, sys.obj$ oi, sys.user$ ui
where oc.owner# = ou.user#
  and oc.con# = c.con#
  and c.obj# = o.obj#
  and c.type# != 8
  and c.type# != 12       /* don't include log groups */
  and c.rcon# = rc.con#(+)
  and c.enabled = oi.obj#(+)
  and oi.obj# = ui.user#(+)
  and rc.owner# = ru.user#(+)
  and c.robj# = ro.obj#(+)
  and (o.owner# = userenv('SCHEMAID')
       or o.obj# in (select obj#
                     from sys.objauth$
                     where grantee# in ( select kzsrorol
                                         from x$kzsro
                                       )
                    )
        or /* user has system privileges */
          exists (select null from v$enabledprivs
                  where priv_number in (-45 /* LOCK ANY TABLE */,
                                        -47 /* SELECT ANY TABLE */,
                                        -48 /* INSERT ANY TABLE */,
                                        -49 /* UPDATE ANY TABLE */,
                                        -50 /* DELETE ANY TABLE */)
                  )
      ); 

