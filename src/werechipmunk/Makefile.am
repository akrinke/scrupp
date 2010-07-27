noinst_LIBRARIES = libwerechipmunk.a
libwerechipmunk_a_SOURCES =               \
  cpArbiter-lua.c                         \
  cpBB-lua.c                              \
  cpBody-lua.c         cpBody-lua.h       \
  cpMisc-lua.c         cpMisc-lua.h       \
  cpSpace-lua.c                           \
  cpVect-lua.c         cpVect-lua.h       \
  cpShape-lua.c        cpShape-lua.h      \
  cpCircleShape-lua.c                     \
  cpPolyShape-lua.c                       \
  cpSegmentShape-lua.c                    \
  constraints/cpConstraint-lua.c          \
  constraints/cpConstraint-lua.h          \
  constraints/cpDampedRotarySpring-lua.c  \
  constraints/cpDampedSpring-lua.c        \
  constraints/cpGearJoint-lua.c           \
  constraints/cpGrooveJoint-lua.c         \
  constraints/cpPinJoint-lua.c            \
  constraints/cpPivotJoint-lua.c          \
  constraints/cpRatchetJoint-lua.c        \
  constraints/cpRotaryLimitJoint-lua.c    \
  constraints/cpSimpleMotor-lua.c         \
  constraints/cpSlideJoint-lua.c          \
  werechipmunk.h
libwerechipmunk_a_CFLAGS = -I$(top_srcdir)/src/chipmunk
AUTOMAKE_OPTIONS = subdir-objects
