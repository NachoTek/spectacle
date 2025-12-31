/*
 *  SPDX-License-Identifier: GPL-2.0-only OR LGPL-2.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 *  Unit Tests for SelectionSnapper
 *  Story 1.2 - Selection Targeting + Refinement
 */

#include <QTest>
#include <QSignalSpy>

#include "Gui/Overlay/SelectionSnapper.h"

class SelectionSnapperTest : public QObject
{
    Q_OBJECT

private:
    SelectionSnapper *m_snapper = nullptr;

private slots:
    void initTestCase()
    {
        m_snapper = new SelectionSnapper(this);
    }

    void cleanupTestCase()
    {
        delete m_snapper;
    }

    void testClickSnapsToTargetBounds()
    {
        // GIVEN: Current selection and target bounds
        QRect current(100, 100, 200, 200);
        QRect target(150, 150, 300, 300);

        QSignalSpy spy(m_snapper, &SelectionSnapper::snapCompleted);

        // WHEN: Snapping to target
        m_snapper->snapTo(current, target);

        // THEN: Should complete snap
        QVERIFY2(spy.wait(200), "Snap should complete within 200ms");

        QRect final = m_snapper->currentBounds();
        QVERIFY2(final == target, "Final bounds should match target");
    }

    void testDoubleClickPreventsSnap()
    {
        // GIVEN: First snap in progress
        QRect current(100, 100, 200, 200);
        QRect target1(150, 150, 300, 300);

        m_snapper->snapTo(current, target1);
        QVERIFY2(m_snapper->isSnapping(), "Should be snapping");

        // WHEN: Rapid second click (within 50ms debounce)
        QTest::qWait(10); // 10ms later
        QRect target2(200, 200, 400, 400);
        m_snapper->snapTo(current, target2);

        // THEN: Second snap should be ignored (debounced)
        // After short wait, bounds should not have moved to target2
        QTest::qWait(20);
        QVERIFY2(m_snapper->isSnapping() || m_snapper->currentBounds() != target2,
                  "Rapid click should be debounced");
    }

    void testSnapAnimationTiming()
    {
        // GIVEN: Bounds to snap
        QRect current(0, 0, 100, 100);
        QRect target(200, 200, 400, 400);

        QSignalSpy spy(m_snapper, &SelectionSnapper::snapCompleted);

        // WHEN: Starting snap
        m_snapper->snapTo(current, target);

        // THEN: Should take approximately 150ms
        QElapsedTimer timer;
        timer.start();

        spy.wait(200);

        qint64 elapsed = timer.elapsed();
        QVERIFY2(elapsed >= 140 && elapsed <= 170, "Animation should take ~150ms (tolerance ±20ms)");
    }

    void testCancelSnapStopsAnimation()
    {
        // GIVEN: Snap in progress
        QRect current(0, 0, 100, 100);
        QRect target(500, 500, 700, 700);

        m_snapper->snapTo(current, target);
        QVERIFY2(m_snapper->isSnapping(), "Should be snapping");

        // WHEN: Canceling snap
        m_snapper->cancelSnap();

        // THEN: Animation should stop
        QVERIFY2(!m_snapper->isSnapping(), "Should not be snapping after cancel");
    }

    void testBoundsChangedDuringAnimation()
    {
        // GIVEN: Snap with large distance
        QRect current(0, 0, 100, 100);
        QRect target(1000, 1000, 1200, 1200);

        QSignalSpy spy(m_snapper, &SelectionSnapper::boundsChanged);

        m_snapper->snapTo(current, target);

        // THEN: Should receive multiple boundsChanged signals during animation
        // (Animation emits at 60fps, so ~9 signals for 150ms)
        QTest::qWait(160);

        QVERIFY2(spy.count() > 5, "Should emit boundsChanged during animation");
    }
};

QTEST_MAIN(SelectionSnapperTest)
#include "SelectionSnapperTest.moc"
