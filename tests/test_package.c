//hawkey
#include "src/query.h"
#include "testsys.h"
#include "test_package.h"

static HyPackage
default_test_package(HySack sack)
{
    HyQuery q = hy_query_create(sack);
    hy_query_filter(q, HY_PKG_NAME, HY_EQ, "penny-lib");
    HyPackageList plist = hy_query_run(q);
    hy_query_free(q);
    fail_unless(hy_packagelist_count(plist) == 1);
    HyPackage pkg = hy_packagelist_get_clone(plist, 0);
    hy_packagelist_free(plist);
    return pkg;
}

static HyPackage
default_yum_package(HySack sack)
{
    HyQuery q = hy_query_create(sack);
    hy_query_filter(q, HY_PKG_NAME, HY_EQ, "mystery");
    HyPackageList plist = hy_query_run(q);
    hy_query_free(q);
    fail_unless(hy_packagelist_count(plist) == 1);
    HyPackage pkg = hy_packagelist_get_clone(plist, 0);
    hy_packagelist_free(plist);
    return pkg;
}

START_TEST(test_refcounting)
{
    HyPackage pkg = default_test_package(test_globals.sack);
    fail_unless(hy_package_link(pkg) != NULL);
    hy_package_free(pkg);
    hy_package_free(pkg);
}
END_TEST

START_TEST(test_package_summary)
{
    HyPackage pkg = default_test_package(test_globals.sack);
    fail_if(strcmp(hy_package_get_summary(pkg), "in my ears"));
    hy_package_free(pkg);
}
END_TEST

START_TEST(test_checksums)
{
    HyPackage pkg = default_yum_package(test_globals.sack);
    int i;
    HyChecksum *csum = hy_package_get_chksum(pkg, &i);
    fail_unless(i == HY_CHKSUM_SHA256);
    // Check the first and last bytes. Those need to match against information
    // in primary.xml.gz.
    fail_unless(csum[0] == 0xb2);
    fail_unless(csum[31] == 0x7a);

    hy_package_free(pkg);
}
END_TEST

Suite *
package_suite(void)
{
    Suite *s = suite_create("Package");
    TCase *tc;

    tc = tcase_create("Core");
    tcase_add_unchecked_fixture(tc, setup, teardown);
    tcase_add_test(tc, test_refcounting);
    tcase_add_test(tc, test_package_summary);
    suite_add_tcase(s, tc);

    tc = tcase_create("WithRealRepo");
    tcase_add_unchecked_fixture(tc, setup_yum, teardown);
    tcase_add_test(tc, test_checksums);
    suite_add_tcase(s, tc);

    return s;
}
