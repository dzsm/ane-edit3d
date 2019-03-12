#ifndef FRAMENAVIGATION_H
#define FRAMENAVIGATION_H

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <assert.h>



namespace FrameNavigation {

template<typename Frame,typename Data> class Sequence;
template<typename Frame,typename Data> class Column;

template<typename Frame,typename Data>
class Column {
    friend class Sequence<Frame,Data>;
private:
    std::vector<Frame*> m_rows;
    Data                m_data;

    int firstRowIndex() const {
        if(isEmpty()) return -1;
        return 0;
    }

    int lastRowIndex() const {
        return numRows()-1;
    }

public:

    Column() {}
    /** \brief Cleans up memory.
     *
     *
     */
    ~Column() {
        for(auto it = m_rows.begin(); it != m_rows.end(); ++it) delete *it;
    }
    /** \brief Number of rows in the column.
     *
     * \return Number of rows.
     *
     */
    int numRows() const {
        return (int)(m_rows.size());
    }
    /** \brief True if empty, that is the number of rows = 0.
     *
     * \return True if empty.
     *
     */
    bool isEmpty() const {
        return m_rows.size() == 0;
    }
    /** \brief Frame referenc at a row index. Assertion checks range.
     *
     * \param row row index
     * \return Frame reference
     *
     */
    Frame & at(int row) {
        assert(row >= 0 && row < numRows());
        return *m_rows[row];
    }
    /** \brief Check if row index in the range.
     *
     * \param row row index
     * \return True if in the range.
     *
     */
    bool checkAt(int row) {
        return row >= 0 && row < numRows();
    }
    /** \brief Create a frame at row index and returns it.
     *
     * \param row row index
     * \return created Frame reference
     *
     */
    Frame & createAt(int row) {
        Frame * pFrame = new Frame();
        assert(row >= 0 && row <= numRows());
        m_rows.insert(m_rows.begin()+row,pFrame);
        return *pFrame;
    }
    /** \brief Remove the frame at row index.
     *
     * \param row row index
     *
     */
    void removeAt(int row) {
        if (isEmpty()) return;

        assert(row >= 0 && row < numRows());
        delete m_rows[row];

        m_rows.erase(m_rows.begin()+row);
    }

    /** \brief Ref to the column data.
     *
     * \param data Column name.
     *
     */
    Data & data() {
        return m_data;
    }
};

template<typename Frame,typename Data>
class Sequence {
private:
    std::vector<Column<Frame,Data>*> m_columns;

    int              m_col;
    int              m_row;
    int              m_step;

    int firstColumnIndex() const {
        if(isEmpty()) return -1;
        return 0;
    }

    int lastColumnIndex() const {
        return numColumns()-1;
    }

public:
    /** \brief Create an empty sequence with step = 1.
     */
    Sequence() : m_col(-1),m_row(-1),m_step(1) {}
    /** \brief Cleans up memory.
     */
    ~Sequence() {
        for(auto it = m_columns.begin(); it != m_columns.end(); ++it) delete *it;
    }

    /** \brief Number of columns in the sequence.
     *
     * \return Number of columns.
     *
     */
    int numColumns() const {
        return (int)(m_columns.size());
    }
    /** \brief True if empty, that is the number of columns = 0.
     *
     * \return True if empty.
     *
     */
    bool isEmpty() const {
        return m_columns.size() == 0;
    }
    /** \brief Total number of the frames in the sequence.
     *
     * \return Number of the frames.
     *
     */
    int size() const {
        int count = 0;
        for(int col = 0; col < numColumns(); col++) {
            assert(m_columns[col] != 0);
            count += m_columns[col]->numRows();
        }
        return count;
    }

    int currentFrameIndex() const {
        int count = 0;
        for(int col = 0; col < m_col; col++) {
            assert(m_columns[col] != 0);
            count += m_columns[col]->numRows();
        }
        count += m_row;
        return count;
    }


    /** \brief The current column index. If empty then it is -1.
     *
     * \return Current column index.
     *
     */
    int currentColumnIndex() const {
        return m_col;
    }
    /** \brief The current row index. If the sequence or the current column is empty then it is -1.
     *   The current column should never be empty.
     * \return Current row index.
     *
     */
    int currentRowIndex() const {
        return m_row;
    }
    /** \brief The step size, by default is 0.
     *
     * \return Step size.
     *
     */
    int step() const {
        return m_step;
    }
    /** \brief Current Frame reference. If empty assertions will fail.
     *
     * \return Frame reference.
     *
     */
    Frame & currentFrame() {
        //if(isEmpty()) return 0;

        assert(m_col >= 0 && m_col < numColumns());
        assert(m_columns[m_col] != 0);

        return m_columns[m_col]->at(m_row);
    }

    Column<Frame,Data> & at(int col) {
        assert(col >= 0 && col < numColumns());
        return *m_columns[col];
    }

    Frame & at(int col,int row) {
        assert(col >= 0 && col < numColumns());
        return m_columns[col]->at(row);
    }

    bool checkAt(int col) {
        return col >= 0 && col < numColumns();
    }

    bool checkAt(int col,int row) {
        bool test = col >= 0 && col < numColumns();
        if (test) return m_columns[col]->checkAt(row);
        return false;
    }

    Column<Frame,Data> & currentColumn() {
        assert(m_col >= 0 && m_col < numColumns());
        return *m_columns[m_col];
    }
    /** \brief Create a new column and in it a new frame and returns it.
     *  It sets the current indecies.
     * \return New Frame reference.
     *
     */
    Frame & createRowWithColumn() {

        m_col++;
        Column<Frame,Data> * p = new Column<Frame,Data>();
        assert(m_col >= 0 && m_col <= numColumns());
        m_columns.insert(m_columns.begin()+m_col,p);
        m_row = 0;
        return p->createAt(m_row);
    }
    /** \brief Create a new frame and returns it. If the sequence is empty it creates a new column first.
     *  It sets the current indecies.
     * \return New Frame reference.
     *
     */
    Frame & createRow() {

        if(isEmpty()) return createRowWithColumn();
        m_row++;
        return m_columns[m_col]->createAt(m_row);

    }
    /** \brief Remove the current frame. It sets the current indecies to the
     *
     * \return New Frame reference.
     *
     */
    void removeCurrentColumn() {

        if(isEmpty()) return;

        delete m_columns[m_col];
        m_columns.erase(m_columns.begin()+m_col);

        if(m_col > lastColumnIndex()) m_col = lastColumnIndex();

        if(isEmpty()) m_row = -1;
        else m_row = m_columns[m_col]->firstRowIndex();

    }
    /** \brief Remove the current frame. If the frame was the only frame it removes the column as well.
     * \return New Frame reference.
     *
     */
    void removeCurrentRow() {
        if(isEmpty()) return;

        Column<Frame,Data> * p = m_columns[m_col];
        p->removeAt(m_row);

        if(p->isEmpty()) {
            removeCurrentColumn();
        } else {
            if(m_row > p->lastRowIndex()) m_row = p->lastRowIndex();
        }
    }

    void moveForward() {
        if(isEmpty()) return;

        m_row += m_step;

        if(m_row > m_columns[m_col]->lastRowIndex()) {
            m_col++;
            if (m_col > lastColumnIndex()) {
                m_col = lastColumnIndex();
                m_row = m_columns[m_col]->lastRowIndex();
            } else {
                m_row = m_columns[m_col]->firstRowIndex();
            }
        }


    }

    void moveBackward() {
        if(isEmpty()) return;

        m_row -= m_step;

        if(m_row < m_columns[m_col]->firstRowIndex()) {
            m_col--;
            if (m_col < firstColumnIndex()) {
                m_col = firstColumnIndex();
                m_row = m_columns[m_col]->firstRowIndex();
            } else {
                m_row = m_columns[m_col]->lastRowIndex();
            }
        }

    }

    void moveFirst() {

        if(isEmpty()) return;
        m_col = firstColumnIndex();
        m_row = m_columns[m_col]->firstRowIndex();
    }

    void moveLast() {
        if(isEmpty()) return;
        m_col = lastColumnIndex();
        m_row = m_columns[m_col]->lastRowIndex();
    }

    void moveNextColumn() {
        if(isEmpty()) return;
        m_col++;
        if (m_col > lastColumnIndex()) {
            m_col = lastColumnIndex();
        } else {
            m_row = m_columns[m_col]->firstRowIndex();
        }
    }

    void movePreviousColumn() {
        if(isEmpty()) return;
        m_col--;
        if (m_col < firstColumnIndex()) {
            m_col = firstColumnIndex();
        } else {
            m_row = m_columns[m_col]->firstRowIndex();
        }
    }

    void moveFrameIndex(int index) {  // todo check crash?
        int count = 0;
        for(int col = 0; col < numColumns(); col++) {
            assert(m_columns[col] != 0);
            count += m_columns[col]->numRows();
            if(count > index) {
                count -= m_columns[col]->numRows();
                m_row = index - count;
                m_col = col;
                break;
              }
        }
        //if(index>=count) moveLast();
    }

    void increaseStep() {
        m_step++;
    }

    void decreaseStep() {
        m_step--;
        if (m_step < 1) m_step = 1;
    }

};


}

#endif
