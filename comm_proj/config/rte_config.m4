
AC_DEFUN([RTE_CONFIG],[

    AS_ECHO([ Hah got this to work])
    AC_ARG_WITH([rte],
                [AC_HELP_STRING([--with-rte(=DIR)],
                                [Where to find the RTE libraries and header
                                files]
                    )],
                [RTE_CFLAGS="-I$withval/include"
                 RTE_LDFLAGS="-L$withval/lib -lrte"
                 AC_SUBST(RTE_CFLAGS, "$RTE_CFLAGS")
                 AC_SUBST(RTE_LDFLAGS, "$RTE_LDFLAGS")
                 ],
                [Do nothing])
])
